static av_always_inline void
dither_8to16(const uint8_t *srcDither, int rot)
{
    if (rot) {
        __asm__ volatile("pxor      %%mm0, %%mm0\n\t"
                         "movq       (%0), %%mm3\n\t"
                         "movq      %%mm3, %%mm4\n\t"
                         "psrlq       $24, %%mm3\n\t"
                         "psllq       $40, %%mm4\n\t"
                         "por       %%mm4, %%mm3\n\t"
                         "movq      %%mm3, %%mm4\n\t"
                         "punpcklbw %%mm0, %%mm3\n\t"
                         "punpckhbw %%mm0, %%mm4\n\t"
                         :: "r"(srcDither)
                         );
    } else {
        __asm__ volatile("pxor      %%mm0, %%mm0\n\t"
                         "movq       (%0), %%mm3\n\t"
                         "movq      %%mm3, %%mm4\n\t"
                         "punpcklbw %%mm0, %%mm3\n\t"
                         "punpckhbw %%mm0, %%mm4\n\t"
                         :: "r"(srcDither)
                         );
    }
}
