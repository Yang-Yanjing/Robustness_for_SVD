static void filter(AVFilterContext *ctx, AVFrame *dstpic,
                   int parity, int tff)
{
    YADIFContext *yadif = ctx->priv;
    ThreadData td = { .frame = dstpic, .parity = parity, .tff = tff };
    int i;
    for (i = 0; i < yadif->csp->nb_components; i++) {
        int w = dstpic->width;
        int h = dstpic->height;
        if (i == 1 || i == 2) {
            w = FF_CEIL_RSHIFT(w, yadif->csp->log2_chroma_w);
            h = FF_CEIL_RSHIFT(h, yadif->csp->log2_chroma_h);
        }
        td.w       = w;
        td.h       = h;
        td.plane   = i;
        ctx->internal->execute(ctx, filter_slice, &td, NULL, FFMIN(h, ctx->graph->nb_threads));
    }
    emms_c();
}
