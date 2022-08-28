static inline uint64_t get_v_trace(AVIOContext *bc, const char *file,
                                   const char *func, int line)
{
    uint64_t v = ffio_read_varlen(bc);
    av_log(NULL, AV_LOG_DEBUG, "get_v %5"PRId64" / %"PRIX64" in %s %s:%d\n",
           v, v, file, func, line);
    return v;
}
