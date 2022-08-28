static inline void create_chrominance_lut(HueContext *h, const int32_t c,
                                          const int32_t s)
{
    int32_t i, j, u, v, new_u, new_v;
    



    for (i = 0; i < 256; i++) {
        for (j = 0; j < 256; j++) {
            
            u = i - 128;
            v = j - 128;
            






            new_u = ((c * u) - (s * v) + (1 << 15) + (128 << 16)) >> 16;
            new_v = ((s * u) + (c * v) + (1 << 15) + (128 << 16)) >> 16;
            
            h->lut_u[i][j] = av_clip_uint8(new_u);
            h->lut_v[i][j] = av_clip_uint8(new_v);
        }
    }
}
