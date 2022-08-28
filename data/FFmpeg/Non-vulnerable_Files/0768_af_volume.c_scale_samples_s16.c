static inline void scale_samples_s16(uint8_t *dst, const uint8_t *src,
                                     int nb_samples, int volume)
{
    int i;
    int16_t *smp_dst       = (int16_t *)dst;
    const int16_t *smp_src = (const int16_t *)src;
    for (i = 0; i < nb_samples; i++)
        smp_dst[i] = av_clip_int16(((int64_t)smp_src[i] * volume + 128) >> 8);
}
