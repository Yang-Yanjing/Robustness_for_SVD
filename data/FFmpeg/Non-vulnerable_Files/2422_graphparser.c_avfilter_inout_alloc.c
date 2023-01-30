AVFilterInOut *avfilter_inout_alloc(void)
{
    return av_mallocz(sizeof(AVFilterInOut));
}
