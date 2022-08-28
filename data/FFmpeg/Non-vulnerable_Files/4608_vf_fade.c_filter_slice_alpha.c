static int filter_slice_alpha(AVFilterContext *ctx, void *arg, int jobnr,
                              int nb_jobs)
{
    FadeContext *s = ctx->priv;
    AVFrame *frame = arg;
    int plane = s->is_packed_rgb ? 0 : A;
    int slice_start = (frame->height *  jobnr   ) / nb_jobs;
    int slice_end   = (frame->height * (jobnr+1)) / nb_jobs;
    int i, j;
    for (i = slice_start; i < slice_end; i++) {
        uint8_t *p = frame->data[plane] + i * frame->linesize[plane] + s->is_packed_rgb*s->rgba_map[A];
        int step = s->is_packed_rgb ? 4 : 1;
        for (j = 0; j < frame->width; j++) {
            


            *p = ((*p - s->black_level) * s->factor + s->black_level_scaled) >> 16;
            p += step;
        }
    }
    return 0;
}
