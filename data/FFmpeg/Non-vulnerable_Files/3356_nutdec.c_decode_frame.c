static int decode_frame(NUTContext *nut, AVPacket *pkt, int frame_code)
{
    AVFormatContext *s = nut->avf;
    AVIOContext *bc    = s->pb;
    int size, stream_id, discard, ret;
    int64_t pts, last_IP_pts;
    StreamContext *stc;
    uint8_t header_idx;
    size = decode_frame_header(nut, &pts, &stream_id, &header_idx, frame_code);
    if (size < 0)
        return size;
    stc = &nut->stream[stream_id];
    if (stc->last_flags & FLAG_KEY)
        stc->skip_until_key_frame = 0;
    discard     = s->streams[stream_id]->discard;
    last_IP_pts = s->streams[stream_id]->last_IP_pts;
    if ((discard >= AVDISCARD_NONKEY && !(stc->last_flags & FLAG_KEY)) ||
        (discard >= AVDISCARD_BIDIR  && last_IP_pts != AV_NOPTS_VALUE &&
         last_IP_pts > pts) ||
        discard >= AVDISCARD_ALL ||
        stc->skip_until_key_frame) {
        avio_skip(bc, size);
        return 1;
    }
    ret = av_new_packet(pkt, size + nut->header_len[header_idx]);
    if (ret < 0)
        return ret;
    memcpy(pkt->data, nut->header[header_idx], nut->header_len[header_idx]);
    pkt->pos = avio_tell(bc); 
    if (stc->last_flags & FLAG_SM_DATA) {
        int sm_size;
        if (read_sm_data(s, bc, pkt, 0, pkt->pos + size) < 0) {
            ret = AVERROR_INVALIDDATA;
            goto fail;
        }
        if (read_sm_data(s, bc, pkt, 1, pkt->pos + size) < 0) {
            ret = AVERROR_INVALIDDATA;
            goto fail;
        }
        sm_size = avio_tell(bc) - pkt->pos;
        size      -= sm_size;
        pkt->size -= sm_size;
    }
    ret = avio_read(bc, pkt->data + nut->header_len[header_idx], size);
    if (ret != size) {
        if (ret < 0)
            goto fail;
    }
    av_shrink_packet(pkt, nut->header_len[header_idx] + ret);
    pkt->stream_index = stream_id;
    if (stc->last_flags & FLAG_KEY)
        pkt->flags |= AV_PKT_FLAG_KEY;
    pkt->pts = pts;
    return 0;
fail:
    av_free_packet(pkt);
    return ret;
}
