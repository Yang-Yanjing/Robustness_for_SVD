static int audio_read_packet(AVFormatContext *s1, AVPacket *pkt)
{
    SndioData *s = s1->priv_data;
    int64_t bdelay, cur_time;
    int ret;
    if ((ret = av_new_packet(pkt, s->buffer_size)) < 0)
        return ret;
    ret = sio_read(s->hdl, pkt->data, pkt->size);
    if (ret == 0 || sio_eof(s->hdl)) {
        av_free_packet(pkt);
        return AVERROR_EOF;
    }
    pkt->size   = ret;
    s->softpos += ret;
    
    cur_time = av_gettime();
    bdelay = ret + s->hwpos - s->softpos;
    
    pkt->pts = cur_time - ((bdelay * 1000000) /
        (s->bps * s->channels * s->sample_rate));
    return 0;
}
