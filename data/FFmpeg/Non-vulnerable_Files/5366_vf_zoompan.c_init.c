AVFILTER_DEFINE_CLASS(zoompan);
static av_cold int init(AVFilterContext *ctx)
{
    ZPContext *s = ctx->priv;
    s->prev_zoom = 1;
    return 0;
}
