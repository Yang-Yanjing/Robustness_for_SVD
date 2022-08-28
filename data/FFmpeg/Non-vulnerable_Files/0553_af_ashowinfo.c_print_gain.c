static void print_gain(AVFilterContext *ctx, const char *str, int32_t gain)
{
    av_log(ctx, AV_LOG_INFO, "%s - ", str);
    if (gain == INT32_MIN)
        av_log(ctx, AV_LOG_INFO, "unknown");
    else
        av_log(ctx, AV_LOG_INFO, "%f", gain / 100000.0f);
    av_log(ctx, AV_LOG_INFO, ", ");
}
