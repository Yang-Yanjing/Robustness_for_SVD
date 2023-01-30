AVFILTER_DEFINE_CLASS(select);
static av_cold int select_init(AVFilterContext *ctx)
{
    int ret;
    if ((ret = init(ctx)) < 0)
        return ret;
    return 0;
}
