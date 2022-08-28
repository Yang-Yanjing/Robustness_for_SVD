static int filter_slice(AVFilterContext *ctx, void *arg, int jobnr, int nb_jobs)
{
    int x, y;
    const CurvesContext *curves = ctx->priv;
    const ThreadData *td = arg;
    const AVFrame *in  = td->in;
    const AVFrame *out = td->out;
    const int direct = out == in;
    const int step = curves->step;
    const uint8_t r = curves->rgba_map[R];
    const uint8_t g = curves->rgba_map[G];
    const uint8_t b = curves->rgba_map[B];
    const uint8_t a = curves->rgba_map[A];
    const int slice_start = (in->height *  jobnr   ) / nb_jobs;
    const int slice_end   = (in->height * (jobnr+1)) / nb_jobs;
    uint8_t       *dst = out->data[0] + slice_start * out->linesize[0];
    const uint8_t *src =  in->data[0] + slice_start *  in->linesize[0];
    for (y = slice_start; y < slice_end; y++) {
        for (x = 0; x < in->width * step; x += step) {
            dst[x + r] = curves->graph[R][src[x + r]];
            dst[x + g] = curves->graph[G][src[x + g]];
            dst[x + b] = curves->graph[B][src[x + b]];
            if (!direct && step == 4)
                dst[x + a] = src[x + a];
        }
        dst += out->linesize[0];
        src += in ->linesize[0];
    }
    return 0;
}
