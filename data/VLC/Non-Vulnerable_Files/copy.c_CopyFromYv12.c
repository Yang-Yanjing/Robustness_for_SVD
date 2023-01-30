}
void CopyFromYv12(picture_t *dst, uint8_t *src[3], size_t src_pitch[3],
                  unsigned width, unsigned height,
                  copy_cache_t *cache)
{
#ifdef CAN_COMPILE_SSE2
    unsigned cpu = vlc_CPU();
    if (vlc_CPU_SSE2())
        return SSE_CopyFromYv12(dst, src, src_pitch, width, height,
                                cache, cpu);
#else
    (void) cache;
#endif
     CopyPlane(dst->p[0].p_pixels, dst->p[0].i_pitch,
               src[0], src_pitch[0], width, height);
     CopyPlane(dst->p[1].p_pixels, dst->p[1].i_pitch,
               src[1], src_pitch[1], width / 2, height / 2);
     CopyPlane(dst->p[2].p_pixels, dst->p[2].i_pitch,
               src[2], src_pitch[2], width / 2, height / 2);
}
