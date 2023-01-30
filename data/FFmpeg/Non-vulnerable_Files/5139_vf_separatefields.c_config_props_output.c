static int config_props_output(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    SeparateFieldsContext *s = ctx->priv;
    AVFilterLink *inlink = ctx->inputs[0];
    s->nb_planes = av_pix_fmt_count_planes(inlink->format);
    if (inlink->h & 1) {
        av_log(ctx, AV_LOG_ERROR, "height must be even\n");
        return AVERROR_INVALIDDATA;
    }
    outlink->time_base.num = inlink->time_base.num;
    outlink->time_base.den = inlink->time_base.den * 2;
    outlink->frame_rate.num = inlink->frame_rate.num * 2;
    outlink->frame_rate.den = inlink->frame_rate.den;
    outlink->w = inlink->w;
    outlink->h = inlink->h / 2;
    return 0;
}
