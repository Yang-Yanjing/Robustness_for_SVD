static av_always_inline void
rgb64ToY_c_template(uint16_t *dst, const uint16_t *src, int width,
                    enum AVPixelFormat origin, int32_t *rgb2yuv)
{
    int32_t ry = rgb2yuv[RY_IDX], gy = rgb2yuv[GY_IDX], by = rgb2yuv[BY_IDX];
    int i;
    for (i = 0; i < width; i++) {
        unsigned int r_b = input_pixel(&src[i*4+0]);
        unsigned int   g = input_pixel(&src[i*4+1]);
        unsigned int b_r = input_pixel(&src[i*4+2]);
        dst[i] = (ry*r + gy*g + by*b + (0x2001<<(RGB2YUV_SHIFT-1))) >> RGB2YUV_SHIFT;
    }
}
