}
static void BlockBlend(uint8_t *dst, size_t dst_pitch,
                       const uint8_t *src, size_t src_pitch,
                       const int16_t *noise,
                       int w, int h)
{
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            dst[y * dst_pitch + x] =
                clip_uint8_vlc(src[y * src_pitch + x] + noise[y * BANK_SIZE +x]);
        }
    }
}
}
static void BlockBlend(uint8_t *dst, size_t dst_pitch,
                       const uint8_t *src, size_t src_pitch,
                       const int16_t *noise,
                       int w, int h)
{
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            dst[y * dst_pitch + x] =
                clip_uint8_vlc(src[y * src_pitch + x] + noise[y * BANK_SIZE +x]);
        }
