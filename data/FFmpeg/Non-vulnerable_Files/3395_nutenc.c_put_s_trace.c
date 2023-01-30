static inline void put_s_trace(AVIOContext *bc, int64_t v, const char *file, const char *func, int line)
{
    av_log(NULL, AV_LOG_DEBUG, "put_s %5"PRId64" / %"PRIX64" in %s %s:%d\n", v, v, file, func, line);
    put_s(bc, v);
}
