static av_cold int init(AVFilterContext *ctx)
{
    SineContext *sine = ctx->priv;
    if (!(sine->sin = av_malloc(sizeof(*sine->sin) << LOG_PERIOD)))
        return AVERROR(ENOMEM);
    sine->dphi = ldexp(sine->frequency, 32) / sine->sample_rate + 0.5;
    make_sin_table(sine->sin);
    if (sine->beep_factor) {
        sine->beep_period = sine->sample_rate;
        sine->beep_length = sine->beep_period / 25;
        sine->dphi_beep = ldexp(sine->beep_factor * sine->frequency, 32) /
                          sine->sample_rate + 0.5;
    }
    return 0;
}
