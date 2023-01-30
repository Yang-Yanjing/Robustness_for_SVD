AVFILTER_DEFINE_CLASS(asyncts);
static av_cold int init(AVFilterContext *ctx)
{
    ASyncContext *s = ctx->priv;
    s->pts         = AV_NOPTS_VALUE;
    s->first_frame = 1;
    return 0;
}
