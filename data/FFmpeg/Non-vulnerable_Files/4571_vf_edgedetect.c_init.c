AVFILTER_DEFINE_CLASS(edgedetect);
static av_cold int init(AVFilterContext *ctx)
{
    EdgeDetectContext *edgedetect = ctx->priv;
    edgedetect->low_u8  = edgedetect->low  * 255. + .5;
    edgedetect->high_u8 = edgedetect->high * 255. + .5;
    return 0;
}
