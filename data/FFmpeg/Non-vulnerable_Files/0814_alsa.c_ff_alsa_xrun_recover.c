int ff_alsa_xrun_recover(AVFormatContext *s1, int err)
{
    AlsaData *s = s1->priv_data;
    snd_pcm_t *handle = s->h;
    av_log(s1, AV_LOG_WARNING, "ALSA buffer xrun.\n");
    if (err == -EPIPE) {
        err = snd_pcm_prepare(handle);
        if (err < 0) {
            av_log(s1, AV_LOG_ERROR, "cannot recover from underrun (snd_pcm_prepare failed: %s)\n", snd_strerror(err));
            return AVERROR(EIO);
        }
    } else if (err == -ESTRPIPE) {
        av_log(s1, AV_LOG_ERROR, "-ESTRPIPE... Unsupported!\n");
        return -1;
    }
    return err;
}
