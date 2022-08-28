static void wait_frame(AVFormatContext *s, AVPacket *pkt)
{
    XCBGrabContext *c = s->priv_data;
    int64_t curtime, delay;
    int64_t frame_time = av_rescale_q(1, c->time_base, AV_TIME_BASE_Q);
    c->time_frame += frame_time;
    for (;;) {
        curtime = av_gettime();
        delay   = c->time_frame - curtime;
        if (delay <= 0)
            break;
        av_usleep(delay);
    }
    pkt->pts = curtime;
}
