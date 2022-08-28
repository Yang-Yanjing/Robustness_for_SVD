av_cold void ff_sws_init_swscale_ppc(SwsContext *c)
{
#if HAVE_ALTIVEC
    enum AVPixelFormat dstFormat = c->dstFormat;
    if (!(av_get_cpu_flags() & AV_CPU_FLAG_ALTIVEC))
        return;
    if (c->srcBpc == 8 && c->dstBpc <= 14) {
        c->hyScale = c->hcScale = hScale_altivec_real;
    }
    if (!is16BPS(dstFormat) && !is9_OR_10BPS(dstFormat) &&
        dstFormat != AV_PIX_FMT_NV12 && dstFormat != AV_PIX_FMT_NV21 &&
        !c->alpPixBuf) {
        c->yuv2planeX = yuv2planeX_altivec;
    }
    

    if (!(c->flags & (SWS_BITEXACT | SWS_FULL_CHR_H_INT)) && !c->alpPixBuf) {
        switch (c->dstFormat) {
        case AV_PIX_FMT_ABGR:
            c->yuv2packedX = ff_yuv2abgr_X_altivec;
            break;
        case AV_PIX_FMT_BGRA:
            c->yuv2packedX = ff_yuv2bgra_X_altivec;
            break;
        case AV_PIX_FMT_ARGB:
            c->yuv2packedX = ff_yuv2argb_X_altivec;
            break;
        case AV_PIX_FMT_RGBA:
            c->yuv2packedX = ff_yuv2rgba_X_altivec;
            break;
        case AV_PIX_FMT_BGR24:
            c->yuv2packedX = ff_yuv2bgr24_X_altivec;
            break;
        case AV_PIX_FMT_RGB24:
            c->yuv2packedX = ff_yuv2rgb24_X_altivec;
            break;
        }
    }
#endif 
}
