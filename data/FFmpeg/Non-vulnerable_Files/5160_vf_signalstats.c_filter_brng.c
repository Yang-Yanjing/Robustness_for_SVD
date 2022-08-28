static int filter_brng(AVFilterContext *ctx, void *arg, int jobnr, int nb_jobs)
{
    ThreadData *td = arg;
    const SignalstatsContext *s = ctx->priv;
    const AVFrame *in = td->in;
    AVFrame *out = td->out;
    const int w = in->width;
    const int h = in->height;
    const int slice_start = (h *  jobnr   ) / nb_jobs;
    const int slice_end   = (h * (jobnr+1)) / nb_jobs;
    int x, y, score = 0;
    for (y = slice_start; y < slice_end; y++) {
        const int yc = y >> s->vsub;
        const uint8_t *pluma    = &in->data[0][y  * in->linesize[0]];
        const uint8_t *pchromau = &in->data[1][yc * in->linesize[1]];
        const uint8_t *pchromav = &in->data[2][yc * in->linesize[2]];
        for (x = 0; x < w; x++) {
            const int xc = x >> s->hsub;
            const int luma    = pluma[x];
            const int chromau = pchromau[xc];
            const int chromav = pchromav[xc];
            const int filt = luma    < 16 || luma    > 235 ||
                chromau < 16 || chromau > 240 ||
                chromav < 16 || chromav > 240;
            score += filt;
            if (out && filt)
                burn_frame(s, out, x, y);
        }
    }
    return score;
}
