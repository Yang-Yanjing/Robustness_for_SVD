static int compute_sat_hue_metrics(AVFilterContext *ctx, void *arg, int jobnr, int nb_jobs)
{
    int i, j;
    ThreadDataHueSatMetrics *td = arg;
    const SignalstatsContext *s = ctx->priv;
    const AVFrame *src = td->src;
    AVFrame *dst_sat = td->dst_sat;
    AVFrame *dst_hue = td->dst_hue;
    const int slice_start = (s->chromah *  jobnr   ) / nb_jobs;
    const int slice_end   = (s->chromah * (jobnr+1)) / nb_jobs;
    const int lsz_u = src->linesize[1];
    const int lsz_v = src->linesize[2];
    const uint8_t *p_u = src->data[1] + slice_start * lsz_u;
    const uint8_t *p_v = src->data[2] + slice_start * lsz_v;
    const int lsz_sat = dst_sat->linesize[0];
    const int lsz_hue = dst_hue->linesize[0];
    uint8_t *p_sat = dst_sat->data[0] + slice_start * lsz_sat;
    uint8_t *p_hue = dst_hue->data[0] + slice_start * lsz_hue;
    for (j = slice_start; j < slice_end; j++) {
        for (i = 0; i < s->chromaw; i++) {
            const int yuvu = p_u[i];
            const int yuvv = p_v[i];
            p_sat[i] = hypot(yuvu - 128, yuvv - 128); 
            ((int16_t*)p_hue)[i] = floor((180 / M_PI) * atan2f(yuvu-128, yuvv-128) + 180);
        }
        p_u   += lsz_u;
        p_v   += lsz_v;
        p_sat += lsz_sat;
        p_hue += lsz_hue;
    }
    return 0;
}
