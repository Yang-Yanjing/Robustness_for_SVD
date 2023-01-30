void av_vlog(void* avcl, int level, const char *fmt, va_list vl)
{
    void (*log_callback)(void*, int, const char*, va_list) = av_log_callback;
    if (log_callback)
        log_callback(avcl, level, fmt, vl);
}
