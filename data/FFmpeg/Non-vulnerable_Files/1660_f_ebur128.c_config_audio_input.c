static int config_audio_input(AVFilterLink *inlink)
{
    AVFilterContext *ctx = inlink->dst;
    EBUR128Context *ebur128 = ctx->priv;
    





    if (ebur128->metadata || (ebur128->peak_mode & PEAK_MODE_TRUE_PEAKS))
        inlink->min_samples =
        inlink->max_samples =
        inlink->partial_buf_size = inlink->sample_rate / 10;
    return 0;
}
