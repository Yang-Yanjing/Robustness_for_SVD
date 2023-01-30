}
static void SSE_CopyPlane(uint8_t *dst, size_t dst_pitch,
                          const uint8_t *src, size_t src_pitch,
                          uint8_t *cache, size_t cache_size,
                          unsigned width, unsigned height, unsigned cpu)
{
    const unsigned w16 = (width+15) & ~15;
    const unsigned hstep = cache_size / w16;
    assert(hstep > 0);
    for (unsigned y = 0; y < height; y += hstep) {
        const unsigned hblock =  __MIN(hstep, height - y);
        /* Copy a bunch of line into our cache */
        CopyFromUswc(cache, w16,
                     src, src_pitch,
                     width, hblock, cpu);
        /* Copy from our cache to the destination */
        Copy2d(dst, dst_pitch,
               cache, w16,
               width, hblock);
        /* */
        src += src_pitch * hblock;
        dst += dst_pitch * hblock;
    }
    asm volatile ("mfence");
}
