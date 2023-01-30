DEFINE_BLEND_EXPR(uint16_t, 16bit, 2)
static int filter_slice(AVFilterContext *ctx, void *arg, int jobnr, int nb_jobs)
{
    ThreadData *td = arg;
    int slice_start = (td->h *  jobnr   ) / nb_jobs;
    int slice_end   = (td->h * (jobnr+1)) / nb_jobs;
    const uint8_t *top    = td->top->data[td->plane];
    const uint8_t *bottom = td->bottom->data[td->plane];
    uint8_t *dst    = td->dst->data[td->plane];
    double values[VAR_VARS_NB];
    values[VAR_N]  = td->inlink->frame_count;
    values[VAR_T]  = td->dst->pts == AV_NOPTS_VALUE ? NAN : td->dst->pts * av_q2d(td->inlink->time_base);
    values[VAR_W]  = td->w;
    values[VAR_H]  = td->h;
    values[VAR_SW] = td->w / (double)td->dst->width;
    values[VAR_SH] = td->h / (double)td->dst->height;
    td->param->blend(top + slice_start * td->top->linesize[td->plane],
                     td->top->linesize[td->plane],
                     bottom + slice_start * td->bottom->linesize[td->plane],
                     td->bottom->linesize[td->plane],
                     dst + slice_start * td->dst->linesize[td->plane],
                     td->dst->linesize[td->plane],
                     td->w, slice_start, slice_end, td->param, &values[0]);
    return 0;
}
