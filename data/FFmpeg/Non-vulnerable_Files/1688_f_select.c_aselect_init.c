AVFILTER_DEFINE_CLASS(aselect);
static av_cold int aselect_init(AVFilterContext *ctx)
{
    SelectContext *select = ctx->priv;
    int ret;
    if ((ret = init(ctx)) < 0)
        return ret;
    if (select->do_scene_detect) {
        av_log(ctx, AV_LOG_ERROR, "Scene detection is ignored in aselect filter\n");
        return AVERROR(EINVAL);
    }
    return 0;
}
