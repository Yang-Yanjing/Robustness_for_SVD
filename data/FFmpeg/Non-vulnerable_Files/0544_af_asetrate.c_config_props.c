static av_cold int config_props(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    ASetRateContext *sr = ctx->priv;
    AVFilterLink *inlink = ctx->inputs[0];
    AVRational intb = ctx->inputs[0]->time_base;
    int inrate = inlink->sample_rate;
    if (intb.num == 1 && intb.den == inrate) {
        outlink->time_base.num = 1;
        outlink->time_base.den = outlink->sample_rate;
    } else {
        outlink->time_base = intb;
        sr->rescale_pts = 1;
        if (av_q2d(intb) > 1.0 / FFMAX(inrate, outlink->sample_rate))
            av_log(ctx, AV_LOG_WARNING, "Time base is inaccurate\n");
    }
    return 0;
}
