av_cold int ff_sndio_open(AVFormatContext *s1, int is_output,
                          const char *audio_device)
{
    SndioData *s = s1->priv_data;
    struct sio_hdl *hdl;
    struct sio_par par;
    hdl = sio_open(audio_device, is_output ? SIO_PLAY : SIO_REC, 0);
    if (!hdl) {
        av_log(s1, AV_LOG_ERROR, "Could not open sndio device\n");
        return AVERROR(EIO);
    }
    sio_initpar(&par);
    par.bits = 16;
    par.sig  = 1;
    par.le   = SIO_LE_NATIVE;
    if (is_output)
        par.pchan = s->channels;
    else
        par.rchan = s->channels;
    par.rate = s->sample_rate;
    if (!sio_setpar(hdl, &par) || !sio_getpar(hdl, &par)) {
        av_log(s1, AV_LOG_ERROR, "Impossible to set sndio parameters, "
               "channels: %d sample rate: %d\n", s->channels, s->sample_rate);
        goto fail;
    }
    if (par.bits != 16 || par.sig != 1 ||
        (is_output  && (par.pchan != s->channels)) ||
        (!is_output && (par.rchan != s->channels)) ||
        (par.rate != s->sample_rate)) {
        av_log(s1, AV_LOG_ERROR, "Could not set appropriate sndio parameters, "
               "channels: %d sample rate: %d\n", s->channels, s->sample_rate);
        goto fail;
    }
    s->buffer_size = par.round * par.bps *
                     (is_output ? par.pchan : par.rchan);
    if (is_output) {
        s->buffer = av_malloc(s->buffer_size);
        if (!s->buffer) {
            av_log(s1, AV_LOG_ERROR, "Could not allocate buffer\n");
            goto fail;
        }
    }
    s->codec_id    = par.le ? AV_CODEC_ID_PCM_S16LE : AV_CODEC_ID_PCM_S16BE;
    s->channels    = is_output ? par.pchan : par.rchan;
    s->sample_rate = par.rate;
    s->bps         = par.bps;
    sio_onmove(hdl, movecb, s);
    if (!sio_start(hdl)) {
        av_log(s1, AV_LOG_ERROR, "Could not start sndio\n");
        goto fail;
    }
    s->hdl = hdl;
    return 0;
fail:
    av_freep(&s->buffer);
    if (hdl)
        sio_close(hdl);
    return AVERROR(EIO);
}
