static int filter_frame(AVFilterLink *inlink, AVFrame *insamples)
{
    SilenceDetectContext *s         = inlink->dst->priv;
    const int nb_channels           = inlink->channels;
    const int srate                 = inlink->sample_rate;
    const int nb_samples            = insamples->nb_samples     * nb_channels;
    const int64_t nb_samples_notify = srate * s->duration * nb_channels;
    
    if (s->last_sample_rate && s->last_sample_rate != srate)
        s->nb_null_samples = srate * s->nb_null_samples / s->last_sample_rate;
    s->last_sample_rate = srate;
    
    s->silencedetect(s, insamples, nb_samples, nb_samples_notify,
                     inlink->time_base);
    return ff_filter_frame(inlink->dst->outputs[0], insamples);
}
