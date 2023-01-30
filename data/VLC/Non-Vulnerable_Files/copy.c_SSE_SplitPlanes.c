}
static void SSE_SplitPlanes(uint8_t *dstu, size_t dstu_pitch,
                            uint8_t *dstv, size_t dstv_pitch,
                            const uint8_t *src, size_t src_pitch,
                            uint8_t *cache, size_t cache_size,
                            unsigned width, unsigned height, unsigned cpu)
{
    const unsigned w2_16 = (2*width+15) & ~15;
    const unsigned hstep = cache_size / w2_16;
    assert(hstep > 0);
    for (unsigned y = 0; y < height; y += hstep) {
        const unsigned hblock =  __MIN(hstep, height - y);
        /* Copy a bunch of line into our cache */
        CopyFromUswc(cache, w2_16, src, src_pitch,
                     2*width, hblock, cpu);
        /* Copy from our cache to the destination */
        SSE_SplitUV(dstu, dstu_pitch, dstv, dstv_pitch,
                    cache, w2_16, width, hblock, cpu);
        /* */
        src  += src_pitch  * hblock;
        dstu += dstu_pitch * hblock;
        dstv += dstv_pitch * hblock;
    }
    asm volatile ("mfence");
}
