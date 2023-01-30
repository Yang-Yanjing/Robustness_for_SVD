static int nut_write_packet(AVFormatContext *s, AVPacket *pkt)
{
    NUTContext *nut    = s->priv_data;
    StreamContext *nus = &nut->stream[pkt->stream_index];
    AVIOContext *bc    = s->pb, *dyn_bc, *sm_bc = NULL;
    FrameCode *fc;
    int64_t coded_pts;
    int best_length, frame_code, flags, needed_flags, i, header_idx;
    int best_header_idx;
    int key_frame = !!(pkt->flags & AV_PKT_FLAG_KEY);
    int store_sp  = 0;
    int ret = 0;
    int sm_size = 0;
    int data_size = pkt->size;
    uint8_t *sm_buf = NULL;
    if (pkt->pts < 0) {
        av_log(s, AV_LOG_ERROR,
               "Negative pts not supported stream %d, pts %"PRId64"\n",
               pkt->stream_index, pkt->pts);
        if (pkt->pts == AV_NOPTS_VALUE)
            av_log(s, AV_LOG_ERROR, "Try to enable the genpts flag\n");
        return AVERROR(EINVAL);
    }
    if (pkt->side_data_elems && nut->version > 3) {
        ret = avio_open_dyn_buf(&sm_bc);
        if (ret < 0)
            return ret;
        ret = write_sm_data(s, sm_bc, pkt, 0);
        if (ret >= 0)
            ret = write_sm_data(s, sm_bc, pkt, 1);
        sm_size = avio_close_dyn_buf(sm_bc, &sm_buf);
        if (ret < 0)
            goto fail;
        data_size += sm_size;
    }
    if (1LL << (20 + 3 * nut->header_count) <= avio_tell(bc))
        write_headers(s, bc);
    if (key_frame && !(nus->last_flags & FLAG_KEY))
        store_sp = 1;
    if (data_size + 30  + avio_tell(bc) >= nut->last_syncpoint_pos + nut->max_distance)
        store_sp = 1;

    if (store_sp &&
        (!(nut->flags & NUT_PIPE) || nut->last_syncpoint_pos == INT_MIN)) {
        int64_t sp_pos = INT64_MAX;
        ff_nut_reset_ts(nut, *nus->time_base, pkt->dts);
        for (i = 0; i < s->nb_streams; i++) {
            AVStream *st   = s->streams[i];
            int64_t dts_tb = av_rescale_rnd(pkt->dts,
                nus->time_base->num * (int64_t)nut->stream[i].time_base->den,
                nus->time_base->den * (int64_t)nut->stream[i].time_base->num,
                AV_ROUND_DOWN);
            int index = av_index_search_timestamp(st, dts_tb,
                                                  AVSEEK_FLAG_BACKWARD);
            if (index >= 0) {
                sp_pos = FFMIN(sp_pos, st->index_entries[index].pos);
                if (!nut->write_index && 2*index > st->nb_index_entries) {
                    memmove(st->index_entries,
                            st->index_entries + index,
                            sizeof(*st->index_entries) * (st->nb_index_entries - index));
                    st->nb_index_entries -=  index;
                }
            }
        }
        nut->last_syncpoint_pos = avio_tell(bc);
        ret                     = avio_open_dyn_buf(&dyn_bc);
        if (ret < 0)
            goto fail;
        put_tt(nut, nus->time_base, dyn_bc, pkt->dts);
        ff_put_v(dyn_bc, sp_pos != INT64_MAX ? (nut->last_syncpoint_pos - sp_pos) >> 4 : 0);
        if (nut->flags & NUT_BROADCAST) {
            put_tt(nut, nus->time_base, dyn_bc,
                   av_rescale_q(av_gettime(), AV_TIME_BASE_Q, *nus->time_base));
        }
        put_packet(nut, bc, dyn_bc, 1, SYNCPOINT_STARTCODE);
        if (nut->write_index) {
        if ((ret = ff_nut_add_sp(nut, nut->last_syncpoint_pos, 0 , pkt->dts)) < 0)
            goto fail;
        if ((1ll<<60) % nut->sp_count == 0)
            for (i=0; i<s->nb_streams; i++) {
                int j;
                StreamContext *nus = &nut->stream[i];
                av_reallocp_array(&nus->keyframe_pts, 2*nut->sp_count, sizeof(*nus->keyframe_pts));
                if (!nus->keyframe_pts) {
                    ret = AVERROR(ENOMEM);
                    goto fail;
                }
                for (j=nut->sp_count == 1 ? 0 : nut->sp_count; j<2*nut->sp_count; j++)
                    nus->keyframe_pts[j] = AV_NOPTS_VALUE;
        }
        }
    }
    av_assert0(nus->last_pts != AV_NOPTS_VALUE);
    coded_pts = pkt->pts & ((1 << nus->msb_pts_shift) - 1);
    if (ff_lsb2full(nus, coded_pts) != pkt->pts)
        coded_pts = pkt->pts + (1 << nus->msb_pts_shift);
    best_header_idx = find_best_header_idx(nut, pkt);
    best_length = INT_MAX;
    frame_code  = -1;
    for (i = 0; i < 256; i++) {
        int length    = 0;
        FrameCode *fc = &nut->frame_code[i];
        int flags     = fc->flags;
        if (flags & FLAG_INVALID)
            continue;
        needed_flags = get_needed_flags(nut, nus, fc, pkt);
        if (flags & FLAG_CODED) {
            length++;
            flags = needed_flags;
        }
        if ((flags & needed_flags) != needed_flags)
            continue;
        if ((flags ^ needed_flags) & FLAG_KEY)
            continue;
        if (flags & FLAG_STREAM_ID)
            length += ff_get_v_length(pkt->stream_index);
        if (data_size % fc->size_mul != fc->size_lsb)
            continue;
        if (flags & FLAG_SIZE_MSB)
            length += ff_get_v_length(data_size / fc->size_mul);
        if (flags & FLAG_CHECKSUM)
            length += 4;
        if (flags & FLAG_CODED_PTS)
            length += ff_get_v_length(coded_pts);
        if (   (flags & FLAG_CODED)
            && nut->header_len[best_header_idx] > nut->header_len[fc->header_idx] + 1) {
            flags |= FLAG_HEADER_IDX;
        }
        if (flags & FLAG_HEADER_IDX) {
            length += 1 - nut->header_len[best_header_idx];
        } else {
            length -= nut->header_len[fc->header_idx];
        }
        length *= 4;
        length += !(flags & FLAG_CODED_PTS);
        length += !(flags & FLAG_CHECKSUM);
        if (length < best_length) {
            best_length = length;
            frame_code  = i;
        }
    }
    av_assert0(frame_code != -1);
    fc           = &nut->frame_code[frame_code];
    flags        = fc->flags;
    needed_flags = get_needed_flags(nut, nus, fc, pkt);
    header_idx   = fc->header_idx;
    ffio_init_checksum(bc, ff_crc04C11DB7_update, 0);
    avio_w8(bc, frame_code);
    if (flags & FLAG_CODED) {
        ff_put_v(bc, (flags ^ needed_flags) & ~(FLAG_CODED));
        flags = needed_flags;
    }
    if (flags & FLAG_STREAM_ID)  ff_put_v(bc, pkt->stream_index);
    if (flags & FLAG_CODED_PTS)  ff_put_v(bc, coded_pts);
    if (flags & FLAG_SIZE_MSB )  ff_put_v(bc, data_size / fc->size_mul);
    if (flags & FLAG_HEADER_IDX) ff_put_v(bc, header_idx = best_header_idx);
    if (flags & FLAG_CHECKSUM)   avio_wl32(bc, ffio_get_checksum(bc));
    else                         ffio_get_checksum(bc);
    if (flags & FLAG_SM_DATA) {
        avio_write(bc, sm_buf, sm_size);
    }
    avio_write(bc, pkt->data + nut->header_len[header_idx], pkt->size - nut->header_len[header_idx]);
    nus->last_flags = flags;
    nus->last_pts   = pkt->pts;
    
    if (flags & FLAG_KEY && !(nut->flags & NUT_PIPE)) {
        av_add_index_entry(
            s->streams[pkt->stream_index],
            nut->last_syncpoint_pos,
            pkt->pts,
            0,
            0,
            AVINDEX_KEYFRAME);
        if (nus->keyframe_pts && nus->keyframe_pts[nut->sp_count] == AV_NOPTS_VALUE)
            nus->keyframe_pts[nut->sp_count] = pkt->pts;
    }
    if (!nut->max_pts_tb || av_compare_ts(nut->max_pts, *nut->max_pts_tb, pkt->pts, *nus->time_base) < 0) {
        nut->max_pts = pkt->pts;
        nut->max_pts_tb = nus->time_base;
    }
fail:
    av_freep(&sm_buf);
    return ret;
}
