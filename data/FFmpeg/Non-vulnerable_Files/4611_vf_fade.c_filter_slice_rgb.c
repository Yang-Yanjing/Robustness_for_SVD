static int filter_slice_rgb(AVFilterContext *ctx, void *arg, int jobnr,
                            int nb_jobs)
{
    FadeContext *s = ctx->priv;
    AVFrame *frame = arg;
    int slice_start = (frame->height *  jobnr   ) / nb_jobs;
    int slice_end   = (frame->height * (jobnr+1)) / nb_jobs;
    if      (s->alpha)    filter_rgb(s, frame, slice_start, slice_end, 1, 4);
    else if (s->bpp == 3) filter_rgb(s, frame, slice_start, slice_end, 0, 3);
    else if (s->bpp == 4) filter_rgb(s, frame, slice_start, slice_end, 0, 4);
    else                  av_assert0(0);
    return 0;
}
