static inline uint64_t get_4cc_trace(AVIOContext *bc, char *file,
                                    char *func, int line)
{
    uint64_t v = get_fourcc(bc);
    av_log(NULL, AV_LOG_DEBUG, "get_fourcc %5"PRId64" / %"PRIX64" in %s %s:%d\n",
           v, v, file, func, line);
    return v;
}
