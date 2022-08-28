static int audio_read_packet(AVFormatContext *s1, AVPacket *pkt)
{
    OSSAudioData *s = s1->priv_data;
    int ret, bdelay;
    int64_t cur_time;
    struct audio_buf_info abufi;
    if ((ret=av_new_packet(pkt, s->frame_size)) < 0)
        return ret;
    ret = read(s->fd, pkt->data, pkt->size);
    if (ret <= 0){
        av_free_packet(pkt);
        pkt->size = 0;
        if (ret<0)  return AVERROR(errno);
        else        return AVERROR_EOF;
    }
    pkt->size = ret;
    
    cur_time = av_gettime();
    bdelay = ret;
    if (ioctl(s->fd, SNDCTL_DSP_GETISPACE, &abufi) == 0) {
        bdelay += abufi.bytes;
    }
    
    cur_time -= (bdelay * 1000000LL) / (s->sample_rate * s->channels);
    
    pkt->pts = cur_time;
    if (s->flip_left && s->channels == 2) {
        int i;
        short *p = (short *) pkt->data;
        for (i = 0; i < ret; i += 4) {
            *p = ~*p;
            p += 2;
        }
    }
    return 0;
}
