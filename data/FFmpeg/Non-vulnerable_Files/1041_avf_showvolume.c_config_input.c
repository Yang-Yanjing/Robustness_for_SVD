static int config_input(AVFilterLink *inlink)
{
    AVFilterContext *ctx = inlink->dst;
    ShowVolumeContext *s = ctx->priv;
    int nb_samples;
    nb_samples = FFMAX(1024, ((double)inlink->sample_rate / av_q2d(s->frame_rate)) + 0.5);
    inlink->partial_buf_size =
    inlink->min_samples =
    inlink->max_samples = nb_samples;
    return 0;
}
