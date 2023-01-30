static int filter_frame(AVFilterLink *inlink, AVFrame *in)
{
    AVFilterContext *ctx = inlink->dst;
    AVFilterLink *outlink = ctx->outputs[0];
    LenscorrectionCtx *rect = (LenscorrectionCtx*)ctx->priv;
    AVFrame *out = ff_get_video_buffer(outlink, outlink->w, outlink->h);
    int plane;
    if (!out) {
        av_frame_free(&in);
        return AVERROR(ENOMEM);
    }
    av_frame_copy_props(out, in);
    for (plane = 0; plane < rect->nb_planes; ++plane) {
        int hsub = plane == 1 || plane == 2 ? rect->hsub : 0;
        int vsub = plane == 1 || plane == 2 ? rect->vsub : 0;
        int hdiv = 1 << hsub;
        int vdiv = 1 << vsub;
        int w = rect->width / hdiv;
        int h = rect->height / vdiv;
        int xcenter = rect->cx * w;
        int ycenter = rect->cy * h;
        int k1 = rect->k1 * (1<<24);
        int k2 = rect->k2 * (1<<24);
        ThreadData td = {
            .in = in,
            .out  = out,
            .w  = w,
            .h  = h,
            .xcenter = xcenter,
            .ycenter = ycenter,
            .plane = plane};
        if (!rect->correction[plane]) {
            int i,j;
            const int64_t r2inv = (4LL<<60) / (w * w + h * h);
            rect->correction[plane] = av_malloc_array(w, h * sizeof(**rect->correction));
            if (!rect->correction[plane])
                return AVERROR(ENOMEM);
            for (j = 0; j < h; j++) {
                const int off_y = j - ycenter;
                const int off_y2 = off_y * off_y;
                for (i = 0; i < w; i++) {
                    const int off_x = i - xcenter;
                    const int64_t r2 = ((off_x * off_x + off_y2) * r2inv + (1LL<<31)) >> 32;
                    const int64_t r4 = (r2 * r2 + (1<<27)) >> 28;
                    const int radius_mult = (r2 * k1 + r4 * k2 + (1LL<<27) + (1LL<<52))>>28;
                    rect->correction[plane][j * w + i] = radius_mult;
                }
            }
        }
        td.correction = rect->correction[plane];
        ctx->internal->execute(ctx, filter_slice, &td, NULL, FFMIN(h, ctx->graph->nb_threads));
    }
    av_frame_free(&in);
    return ff_filter_frame(outlink, out);
}
