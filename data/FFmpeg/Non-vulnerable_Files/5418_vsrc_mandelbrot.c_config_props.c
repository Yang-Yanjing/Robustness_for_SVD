static int config_props(AVFilterLink *inlink)
{
    AVFilterContext *ctx = inlink->src;
    MBContext *mb = ctx->priv;
    if (av_image_check_size(mb->w, mb->h, 0, ctx) < 0)
        return AVERROR(EINVAL);
    inlink->w = mb->w;
    inlink->h = mb->h;
    inlink->time_base = av_inv_q(mb->frame_rate);
    return 0;
}
