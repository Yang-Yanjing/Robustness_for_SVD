int ff_write_chained(AVFormatContext *dst, int dst_stream, AVPacket *pkt,
                     AVFormatContext *src, int interleave)
{
    AVPacket local_pkt;
    int ret;
    local_pkt = *pkt;
    local_pkt.stream_index = dst_stream;
    if (pkt->pts != AV_NOPTS_VALUE)
        local_pkt.pts = av_rescale_q(pkt->pts,
                                     src->streams[pkt->stream_index]->time_base,
                                     dst->streams[dst_stream]->time_base);
    if (pkt->dts != AV_NOPTS_VALUE)
        local_pkt.dts = av_rescale_q(pkt->dts,
                                     src->streams[pkt->stream_index]->time_base,
                                     dst->streams[dst_stream]->time_base);
    if (pkt->duration)
        local_pkt.duration = av_rescale_q(pkt->duration,
                                          src->streams[pkt->stream_index]->time_base,
                                          dst->streams[dst_stream]->time_base);
    if (interleave) ret = av_interleaved_write_frame(dst, &local_pkt);
    else            ret = av_write_frame(dst, &local_pkt);
    pkt->buf = local_pkt.buf;
    pkt->side_data       = local_pkt.side_data;
    pkt->side_data_elems = local_pkt.side_data_elems;
#if FF_API_DESTRUCT_PACKET
FF_DISABLE_DEPRECATION_WARNINGS
    pkt->destruct = local_pkt.destruct;
FF_ENABLE_DEPRECATION_WARNINGS
#endif
    return ret;
}
