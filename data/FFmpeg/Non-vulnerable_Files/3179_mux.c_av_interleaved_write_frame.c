int av_interleaved_write_frame(AVFormatContext *s, AVPacket *pkt)
{
    int ret, flush = 0;
    ret = check_packet(s, pkt);
    if (ret < 0)
        goto fail;
    if (pkt) {
        AVStream *st = s->streams[pkt->stream_index];
        if (s->debug & FF_FDEBUG_TS)
            av_log(s, AV_LOG_TRACE, "av_interleaved_write_frame size:%d dts:%s pts:%s\n",
                pkt->size, av_ts2str(pkt->dts), av_ts2str(pkt->pts));
        if ((ret = compute_pkt_fields2(s, st, pkt)) < 0 && !(s->oformat->flags & AVFMT_NOTIMESTAMPS))
            goto fail;
        if (pkt->dts == AV_NOPTS_VALUE && !(s->oformat->flags & AVFMT_NOTIMESTAMPS)) {
            ret = AVERROR(EINVAL);
            goto fail;
        }
    } else {
        av_log(s, AV_LOG_TRACE, "av_interleaved_write_frame FLUSH\n");
        flush = 1;
    }
    for (;; ) {
        AVPacket opkt;
        int ret = interleave_packet(s, &opkt, pkt, flush);
        if (pkt) {
            memset(pkt, 0, sizeof(*pkt));
            av_init_packet(pkt);
            pkt = NULL;
        }
        if (ret <= 0) 
            return ret;
        ret = write_packet(s, &opkt);
        if (ret >= 0)
            s->streams[opkt.stream_index]->nb_frames++;
        av_free_packet(&opkt);
        if (ret < 0)
            return ret;
        if(s->pb && s->pb->error)
            return s->pb->error;
    }
fail:
    av_packet_unref(pkt);
    return ret;
}
