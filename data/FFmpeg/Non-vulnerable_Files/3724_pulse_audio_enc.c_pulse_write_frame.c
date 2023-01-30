static int pulse_write_frame(AVFormatContext *h, int stream_index,
                             AVFrame **frame, unsigned flags)
{
    AVPacket pkt;
    
    if (flags & AV_WRITE_UNCODED_FRAME_QUERY)
        return av_sample_fmt_is_planar(h->streams[stream_index]->codec->sample_fmt) ?
               AVERROR(EINVAL) : 0;
    pkt.data     = (*frame)->data[0];
    pkt.size     = (*frame)->nb_samples * av_get_bytes_per_sample((*frame)->format) * av_frame_get_channels(*frame);
    pkt.dts      = (*frame)->pkt_dts;
    pkt.duration = av_frame_get_pkt_duration(*frame);
    return pulse_write_packet(h, &pkt);
}
