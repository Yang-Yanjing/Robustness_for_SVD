static int filter_tout(AVFilterContext *ctx, void *arg, int jobnr, int nb_jobs)
{
    ThreadData *td = arg;
    const SignalstatsContext *s = ctx->priv;
    const AVFrame *in = td->in;
    AVFrame *out = td->out;
    const int w = in->width;
    const int h = in->height;
    const int slice_start = (h *  jobnr   ) / nb_jobs;
    const int slice_end   = (h * (jobnr+1)) / nb_jobs;
    const uint8_t *p = in->data[0];
    int lw = in->linesize[0];
    int x, y, score = 0, filt;
    for (y = slice_start; y < slice_end; y++) {
        if (y - 1 < 0 || y + 1 >= h)
            continue;
        
        
#define FILTER(i, j) \
        filter_tout_outlier(p[(y-j) * lw + x + i], \
                            p[    y * lw + x + i], \
                            p[(y+j) * lw + x + i])
#define FILTER3(j) (FILTER(-1, j) && FILTER(0, j) && FILTER(1, j))
        if (y - 2 >= 0 && y + 2 < h) {
            for (x = 1; x < w - 1; x++) {
                filt = FILTER3(2) && FILTER3(1);
                score += filt;
                if (filt && out)
                    burn_frame(s, out, x, y);
            }
        } else {
            for (x = 1; x < w - 1; x++) {
                filt = FILTER3(1);
                score += filt;
                if (filt && out)
                    burn_frame(s, out, x, y);
            }
        }
    }
    return score;
}
