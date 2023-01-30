static void close_filter_param(FilterParam *f)
{
    if (f->pre_filter_context) {
        sws_freeContext(f->pre_filter_context);
        f->pre_filter_context = NULL;
    }
    av_freep(&f->pre_filter_buf);
    av_freep(&f->dist_coeff);
}
