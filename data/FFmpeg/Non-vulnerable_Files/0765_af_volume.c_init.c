static av_cold int init(AVFilterContext *ctx)
{
    VolumeContext *vol = ctx->priv;
    vol->fdsp = avpriv_float_dsp_alloc(0);
    if (!vol->fdsp)
        return AVERROR(ENOMEM);
    return set_expr(&vol->volume_pexpr, vol->volume_expr, ctx);
}
