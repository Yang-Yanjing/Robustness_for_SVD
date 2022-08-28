static int grab_read_packet(AVFormatContext *s1, AVPacket *pkt)
{
    VideoData *s = s1->priv_data;
    int64_t curtime, delay;
    struct timespec ts;
    
    s->time_frame += INT64_C(1000000);
    
    for(;;) {
        curtime = av_gettime();
        delay = s->time_frame * s->time_base.num / s->time_base.den - curtime;
        if (delay <= 0) {
            if (delay < INT64_C(-1000000) * s->time_base.num / s->time_base.den) {
                
                s->time_frame += INT64_C(1000000);
            }
            break;
        }
        ts.tv_sec = delay / 1000000;
        ts.tv_nsec = (delay % 1000000) * 1000;
        nanosleep(&ts, NULL);
    }
    if (av_new_packet(pkt, s->frame_size) < 0)
        return AVERROR(EIO);
    pkt->pts = curtime;
    
    if (s->use_mmap) {
        return v4l_mm_read_picture(s, pkt->data);
    } else {
        if (read(s->fd, pkt->data, pkt->size) != pkt->size)
            return AVERROR(EIO);
        return s->frame_size;
    }
}
