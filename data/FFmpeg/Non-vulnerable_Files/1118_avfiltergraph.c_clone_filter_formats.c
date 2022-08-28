static AVFilterFormats *clone_filter_formats(AVFilterFormats *arg)
{
    AVFilterFormats *a = av_memdup(arg, sizeof(*arg));
    if (a) {
        a->refcount = 0;
        a->refs     = NULL;
        a->formats  = av_memdup(a->formats, sizeof(*a->formats) * a->nb_formats);
        if (!a->formats && arg->formats)
            av_freep(&a);
    }
    return a;
}
