av_cold void ff_get_unscaled_swscale_ppc(SwsContext *c)
{
#if HAVE_ALTIVEC
    if (!(av_get_cpu_flags() & AV_CPU_FLAG_ALTIVEC))
        return;
    if (!(c->srcW & 15) && !(c->flags & SWS_BITEXACT) &&
        c->srcFormat == AV_PIX_FMT_YUV420P) {
        enum AVPixelFormat dstFormat = c->dstFormat;
        
        if (dstFormat == AV_PIX_FMT_YUYV422)
            c->swscale = yv12toyuy2_unscaled_altivec;
        else if (dstFormat == AV_PIX_FMT_UYVY422)
            c->swscale = yv12touyvy_unscaled_altivec;
    }
#endif 
}
