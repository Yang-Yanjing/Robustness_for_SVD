static int fbdev_read_packet(AVFormatContext *avctx, AVPacket *pkt)
{
    FBDevContext *fbdev = avctx->priv_data;
    int64_t curtime, delay;
    struct timespec ts;
    int i, ret;
    uint8_t *pin, *pout;
    if (fbdev->time_frame == AV_NOPTS_VALUE)
        fbdev->time_frame = av_gettime();
    
    while (1) {
        curtime = av_gettime();
        delay = fbdev->time_frame - curtime;
        av_log(avctx, AV_LOG_TRACE,
                "time_frame:%"PRId64" curtime:%"PRId64" delay:%"PRId64"\n",
                fbdev->time_frame, curtime, delay);
        if (delay <= 0) {
            fbdev->time_frame += INT64_C(1000000) / av_q2d(fbdev->framerate_q);
            break;
        }
        if (avctx->flags & AVFMT_FLAG_NONBLOCK)
            return AVERROR(EAGAIN);
        ts.tv_sec  =  delay / 1000000;
        ts.tv_nsec = (delay % 1000000) * 1000;
        while (nanosleep(&ts, &ts) < 0 && errno == EINTR);
    }
    if ((ret = av_new_packet(pkt, fbdev->frame_size)) < 0)
        return ret;
    
    if (ioctl(fbdev->fd, FBIOGET_VSCREENINFO, &fbdev->varinfo) < 0) {
        av_log(avctx, AV_LOG_WARNING,
               "Error refreshing variable info: %s\n", av_err2str(AVERROR(errno)));
    }
    pkt->pts = curtime;
    
    pin = fbdev->data + fbdev->bytes_per_pixel * fbdev->varinfo.xoffset +
                        fbdev->varinfo.yoffset * fbdev->fixinfo.line_length;
    pout = pkt->data;
    for (i = 0; i < fbdev->height; i++) {
        memcpy(pout, pin, fbdev->frame_linesize);
        pin  += fbdev->fixinfo.line_length;
        pout += fbdev->frame_linesize;
    }
    return fbdev->frame_size;
}
