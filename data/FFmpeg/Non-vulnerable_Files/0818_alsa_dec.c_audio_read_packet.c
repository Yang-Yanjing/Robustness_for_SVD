static int audio_read_packet(AVFormatContext *s1, AVPacket *pkt)
{
    AlsaData *s  = s1->priv_data;
    int res;
    int64_t dts;
    snd_pcm_sframes_t delay = 0;
    if (av_new_packet(pkt, s->period_size * s->frame_size) < 0) {
        return AVERROR(EIO);
    }
    while ((res = snd_pcm_readi(s->h, pkt->data, s->period_size)) < 0) {
        if (res == -EAGAIN) {
            av_free_packet(pkt);
            return AVERROR(EAGAIN);
        }
        if (ff_alsa_xrun_recover(s1, res) < 0) {
            av_log(s1, AV_LOG_ERROR, "ALSA read error: %s\n",
                   snd_strerror(res));
            av_free_packet(pkt);
            return AVERROR(EIO);
        }
        ff_timefilter_reset(s->timefilter);
    }
    dts = av_gettime();
    snd_pcm_delay(s->h, &delay);
    dts -= av_rescale(delay + res, 1000000, s->sample_rate);
    pkt->pts = ff_timefilter_update(s->timefilter, dts, s->last_period);
    s->last_period = res;
    pkt->size = res * s->frame_size;
    return 0;
}
