
static inline int16_t *scalarproduct(const int16_t *in, const int16_t *endin, int16_t *out)
{
    int32_t sample;
    int16_t j;
    while (in < endin) {
        sample = 0;
        for (j = 0; j < NUMTAPS; j++)
            sample += in[j] * filt[j];
        *out = av_clip_int16(sample >> 6);
        out++;
        in++;
    }
    return out;
}
