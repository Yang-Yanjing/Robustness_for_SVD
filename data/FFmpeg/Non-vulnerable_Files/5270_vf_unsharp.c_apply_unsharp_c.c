static int apply_unsharp_c(AVFilterContext *ctx, AVFrame *in, AVFrame *out)
{
    AVFilterLink *inlink = ctx->inputs[0];
    UnsharpContext *unsharp = ctx->priv;
    int i, plane_w[3], plane_h[3];
    UnsharpFilterParam *fp[3];
    plane_w[0] = inlink->w;
    plane_w[1] = plane_w[2] = FF_CEIL_RSHIFT(inlink->w, unsharp->hsub);
    plane_h[0] = inlink->h;
    plane_h[1] = plane_h[2] = FF_CEIL_RSHIFT(inlink->h, unsharp->vsub);
    fp[0] = &unsharp->luma;
    fp[1] = fp[2] = &unsharp->chroma;
    for (i = 0; i < 3; i++) {
        apply_unsharp(out->data[i], out->linesize[i], in->data[i], in->linesize[i], plane_w[i], plane_h[i], fp[i]);
    }
    return 0;
}
