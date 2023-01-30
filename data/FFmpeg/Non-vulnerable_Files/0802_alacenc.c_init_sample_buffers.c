static void init_sample_buffers(AlacEncodeContext *s, int channels,
                                uint8_t const *samples[2])
{
    int ch, i;
    int shift = av_get_bytes_per_sample(s->avctx->sample_fmt) * 8 -
                s->avctx->bits_per_raw_sample;
#define COPY_SAMPLES(type) do {                             \
        for (ch = 0; ch < channels; ch++) {                 \
            int32_t       *bptr = s->sample_buf[ch];        \
            const type *sptr = (const type *)samples[ch];   \
            for (i = 0; i < s->frame_size; i++)             \
                bptr[i] = sptr[i] >> shift;                 \
        }                                                   \
    } while (0)
    if (s->avctx->sample_fmt == AV_SAMPLE_FMT_S32P)
        COPY_SAMPLES(int32_t);
    else
        COPY_SAMPLES(int16_t);
}
