static void log_read_interval(const ReadInterval *interval, void *log_ctx, int log_level)
{
    av_log(log_ctx, log_level, "id:%d", interval->id);
    if (interval->has_start) {
        av_log(log_ctx, log_level, " start:%s%s", interval->start_is_offset ? "+" : "",
               av_ts2timestr(interval->start, &AV_TIME_BASE_Q));
    } else {
        av_log(log_ctx, log_level, " start:N/A");
    }
    if (interval->has_end) {
        av_log(log_ctx, log_level, " end:%s", interval->end_is_offset ? "+" : "");
        if (interval->duration_frames)
            av_log(log_ctx, log_level, "#%"PRId64, interval->end);
        else
            av_log(log_ctx, log_level, "%s", av_ts2timestr(interval->end, &AV_TIME_BASE_Q));
    } else {
        av_log(log_ctx, log_level, " end:N/A");
    }
    av_log(log_ctx, log_level, "\n");
}
