static int filter_slice_chroma(AVFilterContext *ctx, void *arg, int jobnr,
                               int nb_jobs)
{
    FadeContext *s = ctx->priv;
    AVFrame *frame = arg;
    int i, j, plane;
    const int width = FF_CEIL_RSHIFT(frame->width, s->hsub);
    const int height= FF_CEIL_RSHIFT(frame->height, s->vsub);
    int slice_start = (height *  jobnr   ) / nb_jobs;
    int slice_end   = (height * (jobnr+1)) / nb_jobs;
    for (plane = 1; plane < 3; plane++) {
        for (i = slice_start; i < slice_end; i++) {
            uint8_t *p = frame->data[plane] + i * frame->linesize[plane];
            for (j = 0; j < width; j++) {
                


                *p = ((*p - 128) * s->factor + 8421367) >> 16;
                p++;
            }
        }
    }
    return 0;
}
