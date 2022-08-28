static av_cold void RENAME(sws_init_swscale)(SwsContext *c)
{
    enum AVPixelFormat dstFormat = c->dstFormat;
    c->use_mmx_vfilter= 0;
    if (!is16BPS(dstFormat) && !is9_OR_10BPS(dstFormat) && dstFormat != AV_PIX_FMT_NV12
        && dstFormat != AV_PIX_FMT_NV21 && !(c->flags & SWS_BITEXACT)) {
            if (c->flags & SWS_ACCURATE_RND) {
                if (!(c->flags & SWS_FULL_CHR_H_INT)) {
                    switch (c->dstFormat) {
                    case AV_PIX_FMT_RGB32:   c->yuv2packedX = RENAME(yuv2rgb32_X_ar);   break;
#if HAVE_6REGS
                    case AV_PIX_FMT_BGR24:   c->yuv2packedX = RENAME(yuv2bgr24_X_ar);   break;
#endif
                    case AV_PIX_FMT_RGB555:  c->yuv2packedX = RENAME(yuv2rgb555_X_ar);  break;
                    case AV_PIX_FMT_RGB565:  c->yuv2packedX = RENAME(yuv2rgb565_X_ar);  break;
                    case AV_PIX_FMT_YUYV422: c->yuv2packedX = RENAME(yuv2yuyv422_X_ar); break;
                    default: break;
                    }
                }
            } else {
                c->use_mmx_vfilter= 1;
                c->yuv2planeX = RENAME(yuv2yuvX    );
                if (!(c->flags & SWS_FULL_CHR_H_INT)) {
                    switch (c->dstFormat) {
                    case AV_PIX_FMT_RGB32:   c->yuv2packedX = RENAME(yuv2rgb32_X);   break;
                    case AV_PIX_FMT_BGR32:   c->yuv2packedX = RENAME(yuv2bgr32_X);   break;
#if HAVE_6REGS
                    case AV_PIX_FMT_BGR24:   c->yuv2packedX = RENAME(yuv2bgr24_X);   break;
#endif
                    case AV_PIX_FMT_RGB555:  c->yuv2packedX = RENAME(yuv2rgb555_X);  break;
                    case AV_PIX_FMT_RGB565:  c->yuv2packedX = RENAME(yuv2rgb565_X);  break;
                    case AV_PIX_FMT_YUYV422: c->yuv2packedX = RENAME(yuv2yuyv422_X); break;
                    default: break;
                    }
                }
            }
        if (!(c->flags & SWS_FULL_CHR_H_INT)) {
            switch (c->dstFormat) {
            case AV_PIX_FMT_RGB32:
                c->yuv2packed1 = RENAME(yuv2rgb32_1);
                c->yuv2packed2 = RENAME(yuv2rgb32_2);
                break;
            case AV_PIX_FMT_BGR24:
                c->yuv2packed1 = RENAME(yuv2bgr24_1);
                c->yuv2packed2 = RENAME(yuv2bgr24_2);
                break;
            case AV_PIX_FMT_RGB555:
                c->yuv2packed1 = RENAME(yuv2rgb555_1);
                c->yuv2packed2 = RENAME(yuv2rgb555_2);
                break;
            case AV_PIX_FMT_RGB565:
                c->yuv2packed1 = RENAME(yuv2rgb565_1);
                c->yuv2packed2 = RENAME(yuv2rgb565_2);
                break;
            case AV_PIX_FMT_YUYV422:
                c->yuv2packed1 = RENAME(yuv2yuyv422_1);
                c->yuv2packed2 = RENAME(yuv2yuyv422_2);
                break;
            default:
                break;
            }
        }
    }
    if (c->srcBpc == 8 && c->dstBpc <= 14) {
    
#if COMPILE_TEMPLATE_MMXEXT
    if (c->flags & SWS_FAST_BILINEAR && c->canMMXEXTBeUsed) {
        c->hyscale_fast = ff_hyscale_fast_mmxext;
        c->hcscale_fast = ff_hcscale_fast_mmxext;
    } else {
#endif 
        c->hyscale_fast = NULL;
        c->hcscale_fast = NULL;
#if COMPILE_TEMPLATE_MMXEXT
    }
#endif 
    }
}
