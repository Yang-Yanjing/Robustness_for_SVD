}
static void SSE_CopyFromYv12(picture_t *dst,
                             uint8_t *src[3], size_t src_pitch[3],
                             unsigned width, unsigned height,
                             copy_cache_t *cache, unsigned cpu)
{
    for (unsigned n = 0; n < 3; n++) {
        const unsigned d = n > 0 ? 2 : 1;
        SSE_CopyPlane(dst->p[n].p_pixels, dst->p[n].i_pitch,
                      src[n], src_pitch[n],
                      cache->buffer, cache->size,
                      (width+d-1)/d, (height+d-1)/d, cpu);
    }
    asm volatile ("emms");
}
