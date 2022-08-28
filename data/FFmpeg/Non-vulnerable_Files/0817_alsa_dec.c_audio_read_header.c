static av_cold int audio_read_header(AVFormatContext *s1)
{
    AlsaData *s = s1->priv_data;
    AVStream *st;
    int ret;
    enum AVCodecID codec_id;
    st = avformat_new_stream(s1, NULL);
    if (!st) {
        av_log(s1, AV_LOG_ERROR, "Cannot add stream\n");
        return AVERROR(ENOMEM);
    }
    codec_id    = s1->audio_codec_id;
    ret = ff_alsa_open(s1, SND_PCM_STREAM_CAPTURE, &s->sample_rate, s->channels,
        &codec_id);
    if (ret < 0) {
        return AVERROR(EIO);
    }
    
    st->codec->codec_type  = AVMEDIA_TYPE_AUDIO;
    st->codec->codec_id    = codec_id;
    st->codec->sample_rate = s->sample_rate;
    st->codec->channels    = s->channels;
    st->codec->frame_size = s->frame_size;
    avpriv_set_pts_info(st, 64, 1, 1000000);  
    
    s->timefilter = ff_timefilter_new(1000000.0 / s->sample_rate,
                                      s->period_size, 1.5E-6);
    if (!s->timefilter)
        goto fail;
    return 0;
fail:
    snd_pcm_close(s->h);
    return AVERROR(EIO);
}
