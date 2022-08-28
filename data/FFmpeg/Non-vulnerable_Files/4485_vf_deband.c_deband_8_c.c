static int deband_8_c(AVFilterContext *ctx, void *arg, int jobnr, int nb_jobs)
{
    DebandContext *s = ctx->priv;
    ThreadData *td = arg;
    AVFrame *in = td->in;
    AVFrame *out = td->out;
    int x, y, p;
    for (p = 0; p < s->nb_components; p++) {
        const uint8_t *src_ptr = (const uint8_t *)in->data[p];
        uint8_t *dst_ptr = (uint8_t *)out->data[p];
        const int dst_linesize = out->linesize[p];
        const int src_linesize = in->linesize[p];
        const int thr = s->thr[p];
        const int start = (s->planeheight[p] *  jobnr   ) / nb_jobs;
        const int end   = (s->planeheight[p] * (jobnr+1)) / nb_jobs;
        const int w = s->planewidth[p] - 1;
        const int h = s->planeheight[p] - 1;
        for (y = start; y < end; y++) {
            const int pos = y * s->planeheight[0];
            for (x = 0; x < s->planewidth[p]; x++) {
                const int x_pos = s->x_pos[pos + x];
                const int y_pos = s->y_pos[pos + x];
                const int ref0 = src_ptr[av_clip(y +  y_pos, 0, h) * src_linesize + av_clip(x +  x_pos, 0, w)];
                const int ref1 = src_ptr[av_clip(y + -y_pos, 0, h) * src_linesize + av_clip(x +  x_pos, 0, w)];
                const int ref2 = src_ptr[av_clip(y + -y_pos, 0, h) * src_linesize + av_clip(x + -x_pos, 0, w)];
                const int ref3 = src_ptr[av_clip(y +  y_pos, 0, h) * src_linesize + av_clip(x + -x_pos, 0, w)];
                const int src0 = src_ptr[y * src_linesize + x];
                if (s->blur) {
                    const int avg = get_avg(ref0, ref1, ref2, ref3);
                    const int diff = FFABS(src0 - avg);
                    dst_ptr[y * dst_linesize + x] = diff < thr ? avg : src0;
                } else {
                    dst_ptr[y * dst_linesize + x] = (FFABS(src0 - ref0) < thr) &&
                                                    (FFABS(src0 - ref1) < thr) &&
                                                    (FFABS(src0 - ref2) < thr) &&
                                                    (FFABS(src0 - ref3) < thr) ? get_avg(ref0, ref1, ref2, ref3) : src0;
                }
            }
        }
    }
    return 0;
}
