static av_cold void uninit(AVFilterContext *ctx)
{
    ReplayGainContext *s = ctx->priv;
    float gain = calc_replaygain(s->histogram);
    av_log(ctx, AV_LOG_INFO, "track_gain = %+.2f dB\n", gain);
    av_log(ctx, AV_LOG_INFO, "track_peak = %.6f\n", s->peak);
}
