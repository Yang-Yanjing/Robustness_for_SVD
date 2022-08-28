static av_cold int init(AVFilterContext *ctx)
{
    ColorMatrixContext *color = ctx->priv;
    if (color->dest == COLOR_MODE_NONE) {
        av_log(ctx, AV_LOG_ERROR, "Unspecified destination color space\n");
        return AVERROR(EINVAL);
    }
    if (color->source == color->dest) {
        av_log(ctx, AV_LOG_ERROR, "Source and destination color space must not be identical\n");
        return AVERROR(EINVAL);
    }
    calc_coefficients(ctx);
    return 0;
}
