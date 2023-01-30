static int encode_frame(AVCodecContext *c, AVFrame *frame)
{
    AVPacket pkt = { 0 };
    int ret, got_output;
    av_init_packet(&pkt);
    ret = avcodec_encode_video2(c, &pkt, frame, &got_output);
    if (ret < 0)
        return ret;
    ret = pkt.size;
    av_free_packet(&pkt);
    return ret;
}
