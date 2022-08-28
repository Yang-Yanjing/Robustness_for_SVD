static int audio_write_frame(AVFormatContext *s1, int stream_index,
                             AVFrame **frame, unsigned flags)
{
    AlsaData *s = s1->priv_data;
    AVPacket pkt;
    
    if ((flags & AV_WRITE_UNCODED_FRAME_QUERY))
        return av_sample_fmt_is_planar(s1->streams[stream_index]->codec->sample_fmt) ?
               AVERROR(EINVAL) : 0;
    
    pkt.data     = (*frame)->data[0];
    pkt.size     = (*frame)->nb_samples * s->frame_size;
    pkt.dts      = (*frame)->pkt_dts;
    pkt.duration = av_frame_get_pkt_duration(*frame);
    return audio_write_packet(s1, &pkt);
}
