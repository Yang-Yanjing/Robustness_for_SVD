static int av_write_uncoded_frame_internal(AVFormatContext *s, int stream_index,
                                           AVFrame *frame, int interleaved)
{
    AVPacket pkt, *pktp;
    av_assert0(s->oformat);
    if (!s->oformat->write_uncoded_frame)
        return AVERROR(ENOSYS);
    if (!frame) {
        pktp = NULL;
    } else {
        pktp = &pkt;
        av_init_packet(&pkt);
        pkt.data = (void *)frame;
        pkt.size         = UNCODED_FRAME_PACKET_SIZE;
        pkt.pts          =
        pkt.dts          = frame->pts;
        pkt.duration     = av_frame_get_pkt_duration(frame);
        pkt.stream_index = stream_index;
        pkt.flags |= AV_PKT_FLAG_UNCODED_FRAME;
    }
    return interleaved ? av_interleaved_write_frame(s, pktp) :
                         av_write_frame(s, pktp);
}
