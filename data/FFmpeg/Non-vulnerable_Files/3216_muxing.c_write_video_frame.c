static int write_video_frame(AVFormatContext *oc, OutputStream *ost)
{
    int ret;
    AVCodecContext *c;
    AVFrame *frame;
    int got_packet = 0;
    c = ost->st->codec;
    frame = get_video_frame(ost);
    if (oc->oformat->flags & AVFMT_RAWPICTURE) {
        
        AVPacket pkt;
        av_init_packet(&pkt);
        if (!frame)
            return 1;
        pkt.flags        |= AV_PKT_FLAG_KEY;
        pkt.stream_index  = ost->st->index;
        pkt.data          = (uint8_t *)frame;
        pkt.size          = sizeof(AVPicture);
        pkt.pts = pkt.dts = frame->pts;
        av_packet_rescale_ts(&pkt, c->time_base, ost->st->time_base);
        ret = av_interleaved_write_frame(oc, &pkt);
    } else {
        AVPacket pkt = { 0 };
        av_init_packet(&pkt);
        
        ret = avcodec_encode_video2(c, &pkt, frame, &got_packet);
        if (ret < 0) {
            fprintf(stderr, "Error encoding video frame: %s\n", av_err2str(ret));
            exit(1);
        }
        if (got_packet) {
            ret = write_frame(oc, &c->time_base, ost->st, &pkt);
        } else {
            ret = 0;
        }
    }
    if (ret < 0) {
        fprintf(stderr, "Error while writing video frame: %s\n", av_err2str(ret));
        exit(1);
    }
    return (frame || got_packet) ? 0 : 1;
}
