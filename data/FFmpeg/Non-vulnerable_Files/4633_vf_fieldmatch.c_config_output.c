static int config_output(AVFilterLink *outlink)
{
    AVFilterContext *ctx  = outlink->src;
    const FieldMatchContext *fm = ctx->priv;
    const AVFilterLink *inlink =
        ctx->inputs[fm->ppsrc ? INPUT_CLEANSRC : INPUT_MAIN];
    outlink->flags |= FF_LINK_FLAG_REQUEST_LOOP;
    outlink->time_base = inlink->time_base;
    outlink->sample_aspect_ratio = inlink->sample_aspect_ratio;
    outlink->frame_rate = inlink->frame_rate;
    outlink->w = inlink->w;
    outlink->h = inlink->h;
    return 0;
}
