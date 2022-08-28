int ff_mov_write_packet(AVFormatContext *s, AVPacket *pkt)
{
    MOVMuxContext *mov = s->priv_data;
    AVIOContext *pb = s->pb;
    MOVTrack *trk = &mov->tracks[pkt->stream_index];
    AVCodecContext *enc = trk->enc;
    unsigned int samples_in_chunk = 0;
    int size = pkt->size, ret = 0;
    uint8_t *reformatted_data = NULL;
    if (trk->entry) {
        int64_t duration = pkt->dts - trk->cluster[trk->entry - 1].dts;
        if (duration < 0 || duration > INT_MAX) {
            av_log(s, AV_LOG_ERROR, "Application provided duration: %"PRId64" / timestamp: %"PRId64" is out of range for mov/mp4 format\n",
                duration, pkt->dts
            );
            pkt->dts = trk->cluster[trk->entry - 1].dts + 1;
            pkt->pts = AV_NOPTS_VALUE;
        }
        if (pkt->duration < 0) {
            av_log(s, AV_LOG_ERROR, "Application provided duration: %d is invalid\n", pkt->duration);
            return AVERROR(EINVAL);
        }
    }
    if (mov->flags & FF_MOV_FLAG_FRAGMENT) {
        int ret;
        if (mov->moov_written || mov->flags & FF_MOV_FLAG_EMPTY_MOOV) {
            if (mov->frag_interleave && mov->fragments > 0) {
                if (trk->entry - trk->entries_flushed >= mov->frag_interleave) {
                    if ((ret = mov_flush_fragment_interleaving(s, trk)) < 0)
                        return ret;
                }
            }
            if (!trk->mdat_buf) {
                if ((ret = avio_open_dyn_buf(&trk->mdat_buf)) < 0)
                    return ret;
            }
            pb = trk->mdat_buf;
        } else {
            if (!mov->mdat_buf) {
                if ((ret = avio_open_dyn_buf(&mov->mdat_buf)) < 0)
                    return ret;
            }
            pb = mov->mdat_buf;
        }
    }
    if (enc->codec_id == AV_CODEC_ID_AMR_NB) {
        
        static const uint16_t packed_size[16] =
            {13, 14, 16, 18, 20, 21, 27, 32, 6, 0, 0, 0, 0, 0, 0, 1};
        int len = 0;
        while (len < size && samples_in_chunk < 100) {
            len += packed_size[(pkt->data[len] >> 3) & 0x0F];
            samples_in_chunk++;
        }
        if (samples_in_chunk > 1) {
            av_log(s, AV_LOG_ERROR, "fatal error, input is not a single packet, implement a AVParser for it\n");
            return -1;
        }
    } else if (enc->codec_id == AV_CODEC_ID_ADPCM_MS ||
               enc->codec_id == AV_CODEC_ID_ADPCM_IMA_WAV) {
        samples_in_chunk = enc->frame_size;
    } else if (trk->sample_size)
        samples_in_chunk = size / trk->sample_size;
    else
        samples_in_chunk = 1;
    
    if (trk->vos_len == 0 && enc->extradata_size > 0 &&
        !TAG_IS_AVCI(trk->tag) &&
        (enc->codec_id != AV_CODEC_ID_DNXHD)) {
        trk->vos_len  = enc->extradata_size;
        trk->vos_data = av_malloc(trk->vos_len);
        if (!trk->vos_data) {
            ret = AVERROR(ENOMEM);
            goto err;
        }
        memcpy(trk->vos_data, enc->extradata, trk->vos_len);
    }
    if (enc->codec_id == AV_CODEC_ID_AAC && pkt->size > 2 &&
        (AV_RB16(pkt->data) & 0xfff0) == 0xfff0) {
        if (!s->streams[pkt->stream_index]->nb_frames) {
            av_log(s, AV_LOG_ERROR, "Malformed AAC bitstream detected: "
                   "use the audio bitstream filter 'aac_adtstoasc' to fix it "
                   "('-bsf:a aac_adtstoasc' option with ffmpeg)\n");
            return -1;
        }
        av_log(s, AV_LOG_WARNING, "aac bitstream error\n");
    }
    if (enc->codec_id == AV_CODEC_ID_H264 && trk->vos_len > 0 && *(uint8_t *)trk->vos_data != 1 && !TAG_IS_AVCI(trk->tag)) {
        
        
        if (trk->hint_track >= 0 && trk->hint_track < mov->nb_streams) {
            ff_avc_parse_nal_units_buf(pkt->data, &reformatted_data,
                                       &size);
            avio_write(pb, reformatted_data, size);
        } else {
            size = ff_avc_parse_nal_units(pb, pkt->data, pkt->size);
        }
    } else if (enc->codec_id == AV_CODEC_ID_HEVC && trk->vos_len > 6 &&
               (AV_RB24(trk->vos_data) == 1 || AV_RB32(trk->vos_data) == 1)) {
        
        if (trk->hint_track >= 0 && trk->hint_track < mov->nb_streams) {
            ff_hevc_annexb2mp4_buf(pkt->data, &reformatted_data, &size, 0, NULL);
            avio_write(pb, reformatted_data, size);
        } else {
            size = ff_hevc_annexb2mp4(pb, pkt->data, pkt->size, 0, NULL);
        }
#if CONFIG_AC3_PARSER
    } else if (enc->codec_id == AV_CODEC_ID_EAC3) {
        size = handle_eac3(mov, pkt, trk);
        if (size < 0)
            return size;
        else if (!size)
            goto end;
        avio_write(pb, pkt->data, size);
#endif
    } else {
        avio_write(pb, pkt->data, size);
    }
    if ((enc->codec_id == AV_CODEC_ID_DNXHD ||
         enc->codec_id == AV_CODEC_ID_AC3) && !trk->vos_len) {
        
        trk->vos_len  = size;
        trk->vos_data = av_malloc(size);
        if (!trk->vos_data) {
            ret = AVERROR(ENOMEM);
            goto err;
        }
        memcpy(trk->vos_data, pkt->data, size);
    }
    if (trk->entry >= trk->cluster_capacity) {
        unsigned new_capacity = 2 * (trk->entry + MOV_INDEX_CLUSTER_SIZE);
        if (av_reallocp_array(&trk->cluster, new_capacity,
                              sizeof(*trk->cluster))) {
            ret = AVERROR(ENOMEM);
            goto err;
        }
        trk->cluster_capacity = new_capacity;
    }
    trk->cluster[trk->entry].pos              = avio_tell(pb) - size;
    trk->cluster[trk->entry].samples_in_chunk = samples_in_chunk;
    trk->cluster[trk->entry].chunkNum         = 0;
    trk->cluster[trk->entry].size             = size;
    trk->cluster[trk->entry].entries          = samples_in_chunk;
    trk->cluster[trk->entry].dts              = pkt->dts;
    if (!trk->entry && trk->start_dts != AV_NOPTS_VALUE) {
        if (!trk->frag_discont) {
            



            trk->cluster[trk->entry].dts = trk->start_dts + trk->track_duration;
            



            if ((mov->flags & FF_MOV_FLAG_DASH && !(mov->flags & FF_MOV_FLAG_GLOBAL_SIDX)) ||
                mov->mode == MODE_ISM)
                pkt->pts = pkt->dts + trk->end_pts - trk->cluster[trk->entry].dts;
        } else {
            


            trk->frag_start = pkt->dts - trk->start_dts;
            trk->end_pts = AV_NOPTS_VALUE;
            trk->frag_discont = 0;
        }
    }
    if (!trk->entry && trk->start_dts == AV_NOPTS_VALUE && !mov->use_editlist &&
        s->avoid_negative_ts == AVFMT_AVOID_NEG_TS_MAKE_ZERO) {
        




        trk->cluster[trk->entry].dts = trk->start_dts = 0;
    }
    if (trk->start_dts == AV_NOPTS_VALUE) {
        trk->start_dts = pkt->dts;
        if (trk->frag_discont) {
            

            trk->frag_start   = pkt->dts;
            trk->start_dts    = 0;
            trk->frag_discont = 0;
        } else if (pkt->dts && mov->moov_written)
            av_log(s, AV_LOG_WARNING,
                   "Track %d starts with a nonzero dts %"PRId64", while the moov "
                   "already has been written. Set the delay_moov flag to handle "
                   "this case.\n",
                   pkt->stream_index, pkt->dts);
    }
    trk->track_duration = pkt->dts - trk->start_dts + pkt->duration;
    trk->last_sample_is_subtitle_end = 0;
    if (pkt->pts == AV_NOPTS_VALUE) {
        av_log(s, AV_LOG_WARNING, "pts has no value\n");
        pkt->pts = pkt->dts;
    }
    if (pkt->dts != pkt->pts)
        trk->flags |= MOV_TRACK_CTTS;
    trk->cluster[trk->entry].cts   = pkt->pts - pkt->dts;
    trk->cluster[trk->entry].flags = 0;
    if (trk->start_cts == AV_NOPTS_VALUE)
        trk->start_cts = pkt->pts - pkt->dts;
    if (trk->end_pts == AV_NOPTS_VALUE)
        trk->end_pts = trk->cluster[trk->entry].dts +
                       trk->cluster[trk->entry].cts + pkt->duration;
    else
        trk->end_pts = FFMAX(trk->end_pts, trk->cluster[trk->entry].dts +
                                           trk->cluster[trk->entry].cts +
                                           pkt->duration);
    if (enc->codec_id == AV_CODEC_ID_VC1) {
        mov_parse_vc1_frame(pkt, trk);
    } else if (pkt->flags & AV_PKT_FLAG_KEY) {
        if (mov->mode == MODE_MOV && enc->codec_id == AV_CODEC_ID_MPEG2VIDEO &&
            trk->entry > 0) { 
            mov_parse_mpeg2_frame(pkt, &trk->cluster[trk->entry].flags);
            if (trk->cluster[trk->entry].flags & MOV_PARTIAL_SYNC_SAMPLE)
                trk->flags |= MOV_TRACK_STPS;
        } else {
            trk->cluster[trk->entry].flags = MOV_SYNC_SAMPLE;
        }
        if (trk->cluster[trk->entry].flags & MOV_SYNC_SAMPLE)
            trk->has_keyframes++;
    }
    trk->entry++;
    trk->sample_count += samples_in_chunk;
    mov->mdat_size    += size;
    if (trk->hint_track >= 0 && trk->hint_track < mov->nb_streams)
        ff_mov_add_hinted_packet(s, pkt, trk->hint_track, trk->entry,
                                 reformatted_data, size);
end:
err:
    av_free(reformatted_data);
    return ret;
}
