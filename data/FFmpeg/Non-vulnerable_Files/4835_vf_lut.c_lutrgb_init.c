AVFILTER_DEFINE_CLASS(lutrgb);
static av_cold int lutrgb_init(AVFilterContext *ctx)
{
    LutContext *s = ctx->priv;
    s->is_rgb = 1;
    return 0;
}
