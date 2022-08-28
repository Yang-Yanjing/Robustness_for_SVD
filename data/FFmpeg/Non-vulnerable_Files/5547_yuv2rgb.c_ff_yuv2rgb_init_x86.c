av_cold SwsFunc ff_yuv2rgb_init_x86(SwsContext *c)
{
#if HAVE_MMX_INLINE && HAVE_6REGS
    int cpu_flags = av_get_cpu_flags();
#if HAVE_MMXEXT_INLINE
    if (INLINE_MMXEXT(cpu_flags)) {
        switch (c->dstFormat) {
        case AV_PIX_FMT_RGB24:
            return yuv420_rgb24_mmxext;
        case AV_PIX_FMT_BGR24:
            return yuv420_bgr24_mmxext;
        }
    }
#endif
    if (INLINE_MMX(cpu_flags)) {
        switch (c->dstFormat) {
            case AV_PIX_FMT_RGB32:
                if (c->srcFormat == AV_PIX_FMT_YUVA420P) {
#if HAVE_7REGS && CONFIG_SWSCALE_ALPHA
                    return yuva420_rgb32_mmx;
#endif
                    break;
                } else
                    return yuv420_rgb32_mmx;
            case AV_PIX_FMT_BGR32:
                if (c->srcFormat == AV_PIX_FMT_YUVA420P) {
#if HAVE_7REGS && CONFIG_SWSCALE_ALPHA
                    return yuva420_bgr32_mmx;
#endif
                    break;
                } else
                    return yuv420_bgr32_mmx;
            case AV_PIX_FMT_RGB24:
                return yuv420_rgb24_mmx;
            case AV_PIX_FMT_BGR24:
                return yuv420_bgr24_mmx;
            case AV_PIX_FMT_RGB565:
                return yuv420_rgb16_mmx;
            case AV_PIX_FMT_RGB555:
                return yuv420_rgb15_mmx;
        }
    }
#endif 
    return NULL;
}
