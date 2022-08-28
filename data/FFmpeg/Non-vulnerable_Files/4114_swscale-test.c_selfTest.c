static void selfTest(uint8_t *ref[4], int refStride[4], int w, int h,
                     enum AVPixelFormat srcFormat_in,
                     enum AVPixelFormat dstFormat_in)
{
    const int flags[] = { SWS_FAST_BILINEAR, SWS_BILINEAR, SWS_BICUBIC,
                          SWS_X, SWS_POINT, SWS_AREA, 0 };
    const int srcW   = w;
    const int srcH   = h;
    const int dstW[] = { srcW - srcW / 3, srcW, srcW + srcW / 3, 0 };
    const int dstH[] = { srcH - srcH / 3, srcH, srcH + srcH / 3, 0 };
    enum AVPixelFormat srcFormat, dstFormat;
    const AVPixFmtDescriptor *desc_src, *desc_dst;
    for (srcFormat = srcFormat_in != AV_PIX_FMT_NONE ? srcFormat_in : 0;
         srcFormat < AV_PIX_FMT_NB; srcFormat++) {
        if (!sws_isSupportedInput(srcFormat) ||
            !sws_isSupportedOutput(srcFormat))
            continue;
        desc_src = av_pix_fmt_desc_get(srcFormat);
        for (dstFormat = dstFormat_in != AV_PIX_FMT_NONE ? dstFormat_in : 0;
             dstFormat < AV_PIX_FMT_NB; dstFormat++) {
            int i, j, k;
            int res = 0;
            if (!sws_isSupportedInput(dstFormat) ||
                !sws_isSupportedOutput(dstFormat))
                continue;
            desc_dst = av_pix_fmt_desc_get(dstFormat);
            printf("%s -> %s\n", desc_src->name, desc_dst->name);
            fflush(stdout);
            for (k = 0; flags[k] && !res; k++)
                for (i = 0; dstW[i] && !res; i++)
                    for (j = 0; dstH[j] && !res; j++)
                        res = doTest(ref, refStride, w, h,
                                     srcFormat, dstFormat,
                                     srcW, srcH, dstW[i], dstH[j], flags[k],
                                     NULL);
            if (dstFormat_in != AV_PIX_FMT_NONE)
                break;
        }
        if (srcFormat_in != AV_PIX_FMT_NONE)
            break;
    }
}
