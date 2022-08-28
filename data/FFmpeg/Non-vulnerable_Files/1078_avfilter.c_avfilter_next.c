const AVFilter *avfilter_next(const AVFilter *prev)
{
    return prev ? prev->next : first_filter;
}
