static int filter_vrep(AVFilterContext *ctx, void *arg, int jobnr, int nb_jobs)
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
    const int lw = in->linesize[0];
    int x, y, score = 0;
    for (y = slice_start; y < slice_end; y++) {
        const int y2lw = (y - VREP_START) * lw;
        const int ylw  =  y               * lw;
        int filt, totdiff = 0;
        if (y < VREP_START)
            continue;
        for (x = 0; x < w; x++)
            totdiff += abs(p[y2lw + x] - p[ylw + x]);
        filt = totdiff < w;
        score += filt;
        if (filt && out)
            for (x = 0; x < w; x++)
                burn_frame(s, out, x, y);
    }
    return score * w;
}
