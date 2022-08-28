static av_cold void haldclut_uninit(AVFilterContext *ctx)
{
    LUT3DContext *lut3d = ctx->priv;
    ff_dualinput_uninit(&lut3d->dinput);
}
