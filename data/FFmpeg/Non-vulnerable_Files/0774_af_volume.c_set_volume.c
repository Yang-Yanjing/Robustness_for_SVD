static int set_volume(AVFilterContext *ctx)
{
    VolumeContext *vol = ctx->priv;
    vol->volume = av_expr_eval(vol->volume_pexpr, vol->var_values, NULL);
    if (isnan(vol->volume)) {
        if (vol->eval_mode == EVAL_MODE_ONCE) {
            av_log(ctx, AV_LOG_ERROR, "Invalid value NaN for volume\n");
            return AVERROR(EINVAL);
        } else {
            av_log(ctx, AV_LOG_WARNING, "Invalid value NaN for volume, setting to 0\n");
            vol->volume = 0;
        }
    }
    vol->var_values[VAR_VOLUME] = vol->volume;
    av_log(ctx, AV_LOG_VERBOSE, "n:%f t:%f pts:%f precision:%s ",
           vol->var_values[VAR_N], vol->var_values[VAR_T], vol->var_values[VAR_PTS],
           precision_str[vol->precision]);
    if (vol->precision == PRECISION_FIXED) {
        vol->volume_i = (int)(vol->volume * 256 + 0.5);
        vol->volume   = vol->volume_i / 256.0;
        av_log(ctx, AV_LOG_VERBOSE, "volume_i:%d/255 ", vol->volume_i);
    }
    av_log(ctx, AV_LOG_VERBOSE, "volume:%f volume_dB:%f\n",
           vol->volume, 20.0*log(vol->volume)/M_LN10);
    volume_init(vol);
    return 0;
}
