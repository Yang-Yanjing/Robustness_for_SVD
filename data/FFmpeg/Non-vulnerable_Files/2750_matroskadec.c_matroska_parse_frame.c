static int matroska_parse_frame(MatroskaDemuxContext *matroska,
                                MatroskaTrack *track, AVStream *st,
                                uint8_t *data, int pkt_size,
                                uint64_t timecode, uint64_t lace_duration,
                                int64_t pos, int is_keyframe,
                                uint8_t *additional, uint64_t additional_id, int additional_size,
                                int64_t discard_padding)
{
    MatroskaTrackEncoding *encodings = track->encodings.elem;
    uint8_t *pkt_data = data;
    int offset = 0, res;
    AVPacket *pkt;
    if (encodings && !encodings->type && encodings->scope & 1) {
        res = matroska_decode_buffer(&pkt_data, &pkt_size, track);
        if (res < 0)
            return res;
    }
    if (st->codec->codec_id == AV_CODEC_ID_WAVPACK) {
        uint8_t *wv_data;
        res = matroska_parse_wavpack(track, pkt_data, &wv_data, &pkt_size);
        if (res < 0) {
            av_log(matroska->ctx, AV_LOG_ERROR,
                   "Error parsing a wavpack block.\n");
            goto fail;
        }
        if (pkt_data != data)
            av_freep(&pkt_data);
        pkt_data = wv_data;
    }
    if (st->codec->codec_id == AV_CODEC_ID_PRORES &&
        AV_RB32(&data[4]) != MKBETAG('i', 'c', 'p', 'f'))
        offset = 8;
    pkt = av_mallocz(sizeof(AVPacket));
    if (!pkt) {
        if (pkt_data != data)
            av_freep(&pkt_data);
        return AVERROR(ENOMEM);
    }
    
    if (av_new_packet(pkt, pkt_size + offset) < 0) {
        av_free(pkt);
        res = AVERROR(ENOMEM);
        goto fail;
    }
    if (st->codec->codec_id == AV_CODEC_ID_PRORES && offset == 8) {
        uint8_t *buf = pkt->data;
        bytestream_put_be32(&buf, pkt_size);
        bytestream_put_be32(&buf, MKBETAG('i', 'c', 'p', 'f'));
    }
    memcpy(pkt->data + offset, pkt_data, pkt_size);
    if (pkt_data != data)
        av_freep(&pkt_data);
    pkt->flags        = is_keyframe;
    pkt->stream_index = st->index;
    if (additional_size > 0) {
        uint8_t *side_data = av_packet_new_side_data(pkt,
                                                     AV_PKT_DATA_MATROSKA_BLOCKADDITIONAL,
                                                     additional_size + 8);
        if (!side_data) {
            av_free_packet(pkt);
            av_free(pkt);
            return AVERROR(ENOMEM);
        }
        AV_WB64(side_data, additional_id);
        memcpy(side_data + 8, additional, additional_size);
    }
    if (discard_padding) {
        uint8_t *side_data = av_packet_new_side_data(pkt,
                                                     AV_PKT_DATA_SKIP_SAMPLES,
                                                     10);
        if (!side_data) {
            av_free_packet(pkt);
            av_free(pkt);
            return AVERROR(ENOMEM);
        }
        AV_WL32(side_data, 0);
        AV_WL32(side_data + 4, av_rescale_q(discard_padding,
                                            (AVRational){1, 1000000000},
                                            (AVRational){1, st->codec->sample_rate}));
    }
    if (track->ms_compat)
        pkt->dts = timecode;
    else
        pkt->pts = timecode;
    pkt->pos = pos;
    if (st->codec->codec_id == AV_CODEC_ID_SUBRIP) {
        







        pkt->convergence_duration = lace_duration;
    }
    if (track->type != MATROSKA_TRACK_TYPE_SUBTITLE ||
        lace_duration <= INT_MAX) {
        






        pkt->duration = lace_duration;
    }
    dynarray_add(&matroska->packets, &matroska->num_packets, pkt);
    matroska->prev_pkt = pkt;
    return 0;
fail:
    if (pkt_data != data)
        av_freep(&pkt_data);
    return res;
}
