}
static void SSE_CopyFromNv12(picture_t *dst,
                             uint8_t *src[2], size_t src_pitch[2],
                             unsigned width, unsigned height,
                             copy_cache_t *cache, unsigned cpu)
{
    SSE_CopyPlane(dst->p[0].p_pixels, dst->p[0].i_pitch,
                  src[0], src_pitch[0],
                  cache->buffer, cache->size,
                  width, height, cpu);
    SSE_SplitPlanes(dst->p[2].p_pixels, dst->p[2].i_pitch,
                    dst->p[1].p_pixels, dst->p[1].i_pitch,
                    src[1], src_pitch[1],
                    cache->buffer, cache->size,
                    (width+1)/2, (height+1)/2, cpu);
    asm volatile ("emms");
}
