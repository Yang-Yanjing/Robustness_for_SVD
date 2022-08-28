static int convert_timestamp(AVFormatContext *ctx, int64_t *ts)
{
    struct video_data *s = ctx->priv_data;
    if (s->ts_mode) {
        int r = init_convert_timestamp(ctx, *ts);
        if (r < 0)
            return r;
    }
#if HAVE_CLOCK_GETTIME && defined(CLOCK_MONOTONIC)
    if (s->timefilter) {
        int64_t nowa = av_gettime();
        int64_t nowm = av_gettime_monotonic();
        ff_timefilter_update(s->timefilter, nowa, nowm - s->last_time_m);
        s->last_time_m = nowm;
        *ts = ff_timefilter_eval(s->timefilter, *ts - nowm);
    }
#endif
    return 0;
}
