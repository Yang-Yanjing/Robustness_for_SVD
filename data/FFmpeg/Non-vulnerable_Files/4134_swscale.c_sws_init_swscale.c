static av_cold void sws_init_swscale(SwsContext *c)
{
    enum AVPixelFormat srcFormat = c->srcFormat;
    ff_sws_init_output_funcs(c, &c->yuv2plane1, &c->yuv2planeX,
                             &c->yuv2nv12cX, &c->yuv2packed1,
                             &c->yuv2packed2, &c->yuv2packedX, &c->yuv2anyX);
    ff_sws_init_input_funcs(c);
    if (c->srcBpc == 8) {
        if (c->dstBpc <= 14) {
            c->hyScale = c->hcScale = hScale8To15_c;
            if (c->flags & SWS_FAST_BILINEAR) {
                c->hyscale_fast = ff_hyscale_fast_c;
                c->hcscale_fast = ff_hcscale_fast_c;
            }
        } else {
            c->hyScale = c->hcScale = hScale8To19_c;
        }
    } else {
        c->hyScale = c->hcScale = c->dstBpc > 14 ? hScale16To19_c
                                                 : hScale16To15_c;
    }
    ff_sws_init_range_convert(c);
    if (!(isGray(srcFormat) || isGray(c->dstFormat) ||
          srcFormat == AV_PIX_FMT_MONOBLACK || srcFormat == AV_PIX_FMT_MONOWHITE))
        c->needs_hcscale = 1;
}
