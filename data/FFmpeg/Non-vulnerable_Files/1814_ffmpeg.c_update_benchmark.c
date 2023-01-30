static void update_benchmark(const char *fmt, ...)
{
    if (do_benchmark_all) {
        int64_t t = getutime();
        va_list va;
        char buf[1024];
        if (fmt) {
            va_start(va, fmt);
            vsnprintf(buf, sizeof(buf), fmt, va);
            va_end(va);
            av_log(NULL, AV_LOG_INFO, "bench: %8"PRIu64" %s \n", t - current_time, buf);
        }
        current_time = t;
    }
}
