VLC_SSE
static void CopyFromUswc(uint8_t *dst, size_t dst_pitch,
                         const uint8_t *src, size_t src_pitch,
                         unsigned width, unsigned height,
                         unsigned cpu)
{
    assert(((intptr_t)dst & 0x0f) == 0 && (dst_pitch & 0x0f) == 0);
    asm volatile ("mfence");
    for (unsigned y = 0; y < height; y++) {
        const unsigned unaligned = (-(uintptr_t)src) & 0x0f;
        unsigned x = 0;
        for (; x < unaligned; x++)
            dst[x] = src[x];
#ifdef CAN_COMPILE_SSE4_1
        if (vlc_CPU_SSE4_1()) {
            if (!unaligned) {
                for (; x+63 < width; x += 64)
                    COPY64(&dst[x], &src[x], "movntdqa", "movdqa");
            } else {
                for (; x+63 < width; x += 64)
                    COPY64(&dst[x], &src[x], "movntdqa", "movdqu");
            }
        } else
#endif
        {
            if (!unaligned) {
                for (; x+63 < width; x += 64)
                    COPY64(&dst[x], &src[x], "movdqa", "movdqa");
            } else {
                for (; x+63 < width; x += 64)
                    COPY64(&dst[x], &src[x], "movdqa", "movdqu");
            }
        }
        for (; x < width; x++)
            dst[x] = src[x];
        src += src_pitch;
        dst += dst_pitch;
    }
}
VLC_SSE
static void CopyFromUswc(uint8_t *dst, size_t dst_pitch,
                         const uint8_t *src, size_t src_pitch,
                         unsigned width, unsigned height,
                         unsigned cpu)
{
    assert(((intptr_t)dst & 0x0f) == 0 && (dst_pitch & 0x0f) == 0);
    asm volatile ("mfence");
    for (unsigned y = 0; y < height; y++) {
        const unsigned unaligned = (-(uintptr_t)src) & 0x0f;
        unsigned x = 0;
        for (; x < unaligned; x++)
            dst[x] = src[x];
#ifdef CAN_COMPILE_SSE4_1
        if (vlc_CPU_SSE4_1()) {
            if (!unaligned) {
                for (; x+63 < width; x += 64)
                    COPY64(&dst[x], &src[x], "movntdqa", "movdqa");
            } else {
                for (; x+63 < width; x += 64)
                    COPY64(&dst[x], &src[x], "movntdqa", "movdqu");
            }
        } else
#endif
        {
            if (!unaligned) {
                for (; x+63 < width; x += 64)
                    COPY64(&dst[x], &src[x], "movdqa", "movdqa");
            } else {
                for (; x+63 < width; x += 64)
                    COPY64(&dst[x], &src[x], "movdqa", "movdqu");
            }
        }
        for (; x < width; x++)
            dst[x] = src[x];
        src += src_pitch;
        dst += dst_pitch;
    }
