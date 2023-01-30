}
static inline uint64_t qsv_mtime_to_timestamp(mtime_t vlc_ts)
{
    return vlc_ts / UINT64_C(100) * UINT64_C(9);
}
