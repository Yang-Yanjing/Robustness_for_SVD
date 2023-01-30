int ff_interleave_packet_per_dts(AVFormatContext *s, AVPacket *out,
                                 AVPacket *pkt, int flush)
{
    AVPacketList *pktl;
    int stream_count = 0;
    int noninterleaved_count = 0;
    int i, ret;
    if (pkt) {
        if ((ret = ff_interleave_add_packet(s, pkt, interleave_compare_dts)) < 0)
            return ret;
    }
    for (i = 0; i < s->nb_streams; i++) {
        if (s->streams[i]->last_in_packet_buffer) {
            ++stream_count;
        } else if (s->streams[i]->codec->codec_type != AVMEDIA_TYPE_ATTACHMENT &&
                   s->streams[i]->codec->codec_id != AV_CODEC_ID_VP8 &&
                   s->streams[i]->codec->codec_id != AV_CODEC_ID_VP9) {
            ++noninterleaved_count;
        }
    }
    if (s->internal->nb_interleaved_streams == stream_count)
        flush = 1;
    if (s->max_interleave_delta > 0 &&
        s->internal->packet_buffer &&
        !flush &&
        s->internal->nb_interleaved_streams == stream_count+noninterleaved_count
    ) {
        AVPacket *top_pkt = &s->internal->packet_buffer->pkt;
        int64_t delta_dts = INT64_MIN;
        int64_t top_dts = av_rescale_q(top_pkt->dts,
                                       s->streams[top_pkt->stream_index]->time_base,
                                       AV_TIME_BASE_Q);
        for (i = 0; i < s->nb_streams; i++) {
            int64_t last_dts;
            const AVPacketList *last = s->streams[i]->last_in_packet_buffer;
            if (!last)
                continue;
            last_dts = av_rescale_q(last->pkt.dts,
                                    s->streams[i]->time_base,
                                    AV_TIME_BASE_Q);
            delta_dts = FFMAX(delta_dts, last_dts - top_dts);
        }
        if (delta_dts > s->max_interleave_delta) {
            av_log(s, AV_LOG_DEBUG,
                   "Delay between the first packet and last packet in the "
                   "muxing queue is %"PRId64" > %"PRId64": forcing output\n",
                   delta_dts, s->max_interleave_delta);
            flush = 1;
        }
    }
    if (stream_count && flush) {
        AVStream *st;
        pktl = s->internal->packet_buffer;
        *out = pktl->pkt;
        st   = s->streams[out->stream_index];
        s->internal->packet_buffer = pktl->next;
        if (!s->internal->packet_buffer)
            s->internal->packet_buffer_end = NULL;
        if (st->last_in_packet_buffer == pktl)
            st->last_in_packet_buffer = NULL;
        av_freep(&pktl);
        return 1;
    } else {
        av_init_packet(out);
        return 0;
    }
}
