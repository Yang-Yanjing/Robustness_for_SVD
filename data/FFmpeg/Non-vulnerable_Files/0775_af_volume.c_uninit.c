static av_cold void uninit(AVFilterContext *ctx)
{
    VolumeContext *vol = ctx->priv;
    av_expr_free(vol->volume_pexpr);
    av_opt_free(vol);
    av_freep(&vol->fdsp);
}
