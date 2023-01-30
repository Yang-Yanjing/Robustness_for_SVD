static int config_output(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    int ret;
    if (ctx->nb_inputs) {
        AVFilterLink *inlink = ctx->inputs[0];
        outlink->format      = inlink->format;
        outlink->sample_rate = inlink->sample_rate;
        ret = 0;
    } else {
        LADSPAContext *s = ctx->priv;
        outlink->sample_rate = s->sample_rate;
        outlink->time_base   = (AVRational){1, s->sample_rate};
        ret = connect_ports(ctx, outlink);
    }
    return ret;
}
