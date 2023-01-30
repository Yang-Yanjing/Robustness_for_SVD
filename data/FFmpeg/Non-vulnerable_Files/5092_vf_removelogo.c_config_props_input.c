static int config_props_input(AVFilterLink *inlink)
{
    AVFilterContext *ctx = inlink->dst;
    RemovelogoContext *s = ctx->priv;
    if (inlink->w != s->mask_w || inlink->h != s->mask_h) {
        av_log(ctx, AV_LOG_INFO,
               "Mask image size %dx%d does not match with the input video size %dx%d\n",
               s->mask_w, s->mask_h, inlink->w, inlink->h);
        return AVERROR(EINVAL);
    }
    return 0;
}
