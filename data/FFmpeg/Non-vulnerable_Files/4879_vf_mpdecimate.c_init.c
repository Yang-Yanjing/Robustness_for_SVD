static av_cold int init(AVFilterContext *ctx)
{
    DecimateContext *decimate = ctx->priv;
    decimate->sad = av_pixelutils_get_sad_fn(3, 3, 0, ctx); 
    if (!decimate->sad)
        return AVERROR(EINVAL);
    av_log(ctx, AV_LOG_VERBOSE, "max_drop_count:%d hi:%d lo:%d frac:%f\n",
           decimate->max_drop_count, decimate->hi, decimate->lo, decimate->frac);
    return 0;
}
