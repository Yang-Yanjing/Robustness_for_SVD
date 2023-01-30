AVFILTER_DEFINE_CLASS(compand);
static av_cold int init(AVFilterContext *ctx)
{
    CompandContext *s = ctx->priv;
    s->pts            = AV_NOPTS_VALUE;
    return 0;
}
