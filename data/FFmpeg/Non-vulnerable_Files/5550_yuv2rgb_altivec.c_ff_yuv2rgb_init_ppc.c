av_cold SwsFunc ff_yuv2rgb_init_ppc(SwsContext *c)
{
#if HAVE_ALTIVEC
    if (!(av_get_cpu_flags() & AV_CPU_FLAG_ALTIVEC))
        return NULL;
    






    if ((c->srcW & 0xf) != 0)
        return NULL;
    switch (c->srcFormat) {
    case AV_PIX_FMT_YUV410P:
    case AV_PIX_FMT_YUV420P:
    
    case AV_PIX_FMT_GRAY8:
    case AV_PIX_FMT_NV12:
    case AV_PIX_FMT_NV21:
        if ((c->srcH & 0x1) != 0)
            return NULL;
        switch (c->dstFormat) {
        case AV_PIX_FMT_RGB24:
            av_log(c, AV_LOG_WARNING, "ALTIVEC: Color Space RGB24\n");
            return altivec_yuv2_rgb24;
        case AV_PIX_FMT_BGR24:
            av_log(c, AV_LOG_WARNING, "ALTIVEC: Color Space BGR24\n");
            return altivec_yuv2_bgr24;
        case AV_PIX_FMT_ARGB:
            av_log(c, AV_LOG_WARNING, "ALTIVEC: Color Space ARGB\n");
            return altivec_yuv2_argb;
        case AV_PIX_FMT_ABGR:
            av_log(c, AV_LOG_WARNING, "ALTIVEC: Color Space ABGR\n");
            return altivec_yuv2_abgr;
        case AV_PIX_FMT_RGBA:
            av_log(c, AV_LOG_WARNING, "ALTIVEC: Color Space RGBA\n");
            return altivec_yuv2_rgba;
        case AV_PIX_FMT_BGRA:
            av_log(c, AV_LOG_WARNING, "ALTIVEC: Color Space BGRA\n");
            return altivec_yuv2_bgra;
        default: return NULL;
        }
        break;
    case AV_PIX_FMT_UYVY422:
        switch (c->dstFormat) {
        case AV_PIX_FMT_BGR32:
            av_log(c, AV_LOG_WARNING, "ALTIVEC: Color Space UYVY -> RGB32\n");
            return altivec_uyvy_rgb32;
        default: return NULL;
        }
        break;
    }
#endif 
    return NULL;
}
