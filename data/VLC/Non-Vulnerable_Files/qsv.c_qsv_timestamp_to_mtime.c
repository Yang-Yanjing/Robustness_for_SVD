static block_t *Encode(encoder_t *, picture_t *);
static inline mtime_t qsv_timestamp_to_mtime(int64_t mfx_ts)
{
    return mfx_ts / INT64_C(9) * INT64_C(100);
}
