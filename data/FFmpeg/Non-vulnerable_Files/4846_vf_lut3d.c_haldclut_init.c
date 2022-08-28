static av_cold int haldclut_init(AVFilterContext *ctx)
{
    LUT3DContext *lut3d = ctx->priv;
    lut3d->dinput.process = update_apply_clut;
    return 0;
}
