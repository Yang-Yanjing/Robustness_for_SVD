static av_cold int init_alphaextract(AVFilterContext *ctx)
{
    ExtractPlanesContext *s = ctx->priv;
    s->requested_planes = PLANE_A;
    return init(ctx);
}
