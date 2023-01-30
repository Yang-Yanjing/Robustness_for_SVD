static int request_frame(AVFilterLink *outlink)
{
    SineContext *sine = outlink->src->priv;
    AVFrame *frame;
    int i, nb_samples = sine->samples_per_frame;
    int16_t *samples;
    if (sine->duration) {
        nb_samples = FFMIN(nb_samples, sine->duration - sine->pts);
        av_assert1(nb_samples >= 0);
        if (!nb_samples)
            return AVERROR_EOF;
    }
    if (!(frame = ff_get_audio_buffer(outlink, nb_samples)))
        return AVERROR(ENOMEM);
    samples = (int16_t *)frame->data[0];
    for (i = 0; i < nb_samples; i++) {
        samples[i] = sine->sin[sine->phi >> (32 - LOG_PERIOD)];
        sine->phi += sine->dphi;
        if (sine->beep_index < sine->beep_length) {
            samples[i] += sine->sin[sine->phi_beep >> (32 - LOG_PERIOD)] << 1;
            sine->phi_beep += sine->dphi_beep;
        }
        if (++sine->beep_index == sine->beep_period)
            sine->beep_index = 0;
    }
    frame->pts = sine->pts;
    sine->pts += nb_samples;
    return ff_filter_frame(outlink, frame);
}
