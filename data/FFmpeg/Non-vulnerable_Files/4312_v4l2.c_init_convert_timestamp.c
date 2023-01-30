static int init_convert_timestamp(AVFormatContext *ctx, int64_t ts)
{
    struct video_data *s = ctx->priv_data;
    int64_t now;
    now = av_gettime();
    if (s->ts_mode == V4L_TS_ABS &&
        ts <= now + 1 * AV_TIME_BASE && ts >= now - 10 * AV_TIME_BASE) {
        av_log(ctx, AV_LOG_INFO, "Detected absolute timestamps\n");
        s->ts_mode = V4L_TS_CONVERT_READY;
        return 0;
    }
#if HAVE_CLOCK_GETTIME && defined(CLOCK_MONOTONIC)
    if (ctx->streams[0]->avg_frame_rate.num) {
        now = av_gettime_monotonic();
        if (s->ts_mode == V4L_TS_MONO2ABS ||
            (ts <= now + 1 * AV_TIME_BASE && ts >= now - 10 * AV_TIME_BASE)) {
            AVRational tb = {AV_TIME_BASE, 1};
            int64_t period = av_rescale_q(1, tb, ctx->streams[0]->avg_frame_rate);
            av_log(ctx, AV_LOG_INFO, "Detected monotonic timestamps, converting\n");
            
            s->timefilter = ff_timefilter_new(1, period, 1.0E-6);
            if (!s->timefilter)
                return AVERROR(ENOMEM);
            s->ts_mode = V4L_TS_CONVERT_READY;
            return 0;
        }
    }
#endif
    av_log(ctx, AV_LOG_ERROR, "Unknown timestamps\n");
    return AVERROR(EIO);
}
