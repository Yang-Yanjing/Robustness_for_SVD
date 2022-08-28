static void print_peak(AVFilterContext *ctx, const char *str, uint32_t peak)
{
    av_log(ctx, AV_LOG_INFO, "%s - ", str);
    if (!peak)
        av_log(ctx, AV_LOG_INFO, "unknown");
    else
        av_log(ctx, AV_LOG_INFO, "%f", (float)peak / UINT32_MAX);
    av_log(ctx, AV_LOG_INFO, ", ");
}
