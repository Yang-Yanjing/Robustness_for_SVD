static inline void ff_put_v_trace(AVIOContext *bc, uint64_t v, const char *file,
                                  const char *func, int line)
{
    av_log(NULL, AV_LOG_DEBUG, "ff_put_v %5"PRId64" / %"PRIX64" in %s %s:%d\n", v, v, file, func, line);
    ff_put_v(bc, v);
}
