void avfilter_inout_free(AVFilterInOut **inout)
{
    while (*inout) {
        AVFilterInOut *next = (*inout)->next;
        av_freep(&(*inout)->name);
        av_freep(inout);
        *inout = next;
    }
}
