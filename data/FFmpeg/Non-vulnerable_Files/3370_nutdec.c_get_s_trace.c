static inline int64_t get_s_trace(AVIOContext *bc, const char *file,
                                  const char *func, int line)
{
    int64_t v = get_s(bc);
    av_log(NULL, AV_LOG_DEBUG, "get_s %5"PRId64" / %"PRIX64" in %s %s:%d\n",
           v, v, file, func, line);
    return v;
}
