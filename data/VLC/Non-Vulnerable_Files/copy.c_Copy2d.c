VLC_SSE
static void Copy2d(uint8_t *dst, size_t dst_pitch,
                   const uint8_t *src, size_t src_pitch,
                   unsigned width, unsigned height)
{
    assert(((intptr_t)src & 0x0f) == 0 && (src_pitch & 0x0f) == 0);
    asm volatile ("mfence");
    for (unsigned y = 0; y < height; y++) {
        unsigned x = 0;
        bool unaligned = ((intptr_t)dst & 0x0f) != 0;
        if (!unaligned) {
            for (; x+63 < width; x += 64)
                COPY64(&dst[x], &src[x], "movdqa", "movntdq");
        } else {
            for (; x+63 < width; x += 64)
                COPY64(&dst[x], &src[x], "movdqa", "movdqu");
        }
        for (; x < width; x++)
            dst[x] = src[x];
        src += src_pitch;
        dst += dst_pitch;
    }
}
VLC_SSE
static void Copy2d(uint8_t *dst, size_t dst_pitch,
                   const uint8_t *src, size_t src_pitch,
                   unsigned width, unsigned height)
{
    assert(((intptr_t)src & 0x0f) == 0 && (src_pitch & 0x0f) == 0);
    asm volatile ("mfence");
    for (unsigned y = 0; y < height; y++) {
        unsigned x = 0;
        bool unaligned = ((intptr_t)dst & 0x0f) != 0;
        if (!unaligned) {
            for (; x+63 < width; x += 64)
                COPY64(&dst[x], &src[x], "movdqa", "movntdq");
        } else {
            for (; x+63 < width; x += 64)
                COPY64(&dst[x], &src[x], "movdqa", "movdqu");
        }
        for (; x < width; x++)
            dst[x] = src[x];
        src += src_pitch;
        dst += dst_pitch;
    }
