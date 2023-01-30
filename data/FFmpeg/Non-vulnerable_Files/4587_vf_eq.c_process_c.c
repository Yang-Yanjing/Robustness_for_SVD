static void process_c(EQParameters *param, uint8_t *dst, int dst_stride,
                      const uint8_t *src, int src_stride, int w, int h)
{
    int x, y, pel;
    int contrast = (int) (param->contrast * 256 * 16);
    int brightness = ((int) (100.0 * param->brightness + 100.0) * 511) / 200 - 128 - contrast / 32;
    for (y = 0; y < h; y++) {
        for (x = 0; x < w; x++) {
            pel = ((src[y * src_stride + x] * contrast) >> 12) + brightness;
            if (pel & ~255)
                pel = (-pel) >> 31;
            dst[y * dst_stride + x] = pel;
        }
    }
}
