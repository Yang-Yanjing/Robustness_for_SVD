static int link_filter(AVFilterContext *src, int srcpad,
                       AVFilterContext *dst, int dstpad,
                       void *log_ctx)
{
    int ret;
    if ((ret = avfilter_link(src, srcpad, dst, dstpad))) {
        av_log(log_ctx, AV_LOG_ERROR,
               "Cannot create the link %s:%d -> %s:%d\n",
               src->filter->name, srcpad, dst->filter->name, dstpad);
        return ret;
    }
    return 0;
}
