static int filter_slice_luma(AVFilterContext *ctx, void *arg, int jobnr,
                             int nb_jobs)
{
    FadeContext *s = ctx->priv;
    AVFrame *frame = arg;
    int slice_start = (frame->height *  jobnr   ) / nb_jobs;
    int slice_end   = (frame->height * (jobnr+1)) / nb_jobs;
    int i, j;
    for (i = slice_start; i < slice_end; i++) {
        uint8_t *p = frame->data[0] + i * frame->linesize[0];
        for (j = 0; j < frame->width * s->bpp; j++) {
            


            *p = ((*p - s->black_level) * s->factor + s->black_level_scaled) >> 16;
            p++;
        }
    }
    return 0;
}
