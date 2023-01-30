void av_log_set_callback(void (*callback)(void*, int, const char*, va_list))
{
    av_log_callback = callback;
}
