static inline void create_luma_lut(HueContext *h)
{
    const float b = h->brightness;
    int i;
    for (i = 0; i < 256; i++) {
        h->lut_l[i] = av_clip_uint8(i + b * 25.5);
    }
}
