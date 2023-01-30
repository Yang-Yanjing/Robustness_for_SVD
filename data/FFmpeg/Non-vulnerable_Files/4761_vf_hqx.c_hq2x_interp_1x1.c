static av_always_inline uint32_t hq2x_interp_1x1(const uint32_t *r2y, int k,
                                                 const uint32_t *w,
                                                 int p0, int p1, int p2,
                                                 int p3, int p4, int p5,
                                                 int p6, int p7, int p8)
{
    INTERP_BOOTSTRAP(0);
    if ((P(0xbf,0x37) || P(0xdb,0x13)) && WDIFF(w1, w5))
        return interp_2px(w4, 3, w3, 1, 2);
    if ((P(0xdb,0x49) || P(0xef,0x6d)) && WDIFF(w7, w3))
        return interp_2px(w4, 3, w1, 1, 2);
    if ((P(0x0b,0x0b) || P(0xfe,0x4a) || P(0xfe,0x1a)) && WDIFF(w3, w1))
        return w4;
    if ((P(0x6f,0x2a) || P(0x5b,0x0a) || P(0xbf,0x3a) || P(0xdf,0x5a) ||
         P(0x9f,0x8a) || P(0xcf,0x8a) || P(0xef,0x4e) || P(0x3f,0x0e) ||
         P(0xfb,0x5a) || P(0xbb,0x8a) || P(0x7f,0x5a) || P(0xaf,0x8a) ||
         P(0xeb,0x8a)) && WDIFF(w3, w1))
        return interp_2px(w4, 3, w0, 1, 2);
    if (P(0x0b,0x08))
        return interp_3px(w4, 2, w0, 1, w1, 1, 2);
    if (P(0x0b,0x02))
        return interp_3px(w4, 2, w0, 1, w3, 1, 2);
    if (P(0x2f,0x2f))
        return interp_3px(w4, 14, w3, 1, w1, 1, 4);
    if (P(0xbf,0x37) || P(0xdb,0x13))
        return interp_3px(w4, 5, w1, 2, w3, 1, 3);
    if (P(0xdb,0x49) || P(0xef,0x6d))
        return interp_3px(w4, 5, w3, 2, w1, 1, 3);
    if (P(0x1b,0x03) || P(0x4f,0x43) || P(0x8b,0x83) || P(0x6b,0x43))
        return interp_2px(w4, 3, w3, 1, 2);
    if (P(0x4b,0x09) || P(0x8b,0x89) || P(0x1f,0x19) || P(0x3b,0x19))
        return interp_2px(w4, 3, w1, 1, 2);
    if (P(0x7e,0x2a) || P(0xef,0xab) || P(0xbf,0x8f) || P(0x7e,0x0e))
        return interp_3px(w4, 2, w3, 3, w1, 3, 3);
    if (P(0xfb,0x6a) || P(0x6f,0x6e) || P(0x3f,0x3e) || P(0xfb,0xfa) ||
        P(0xdf,0xde) || P(0xdf,0x1e))
        return interp_2px(w4, 3, w0, 1, 2);
    if (P(0x0a,0x00) || P(0x4f,0x4b) || P(0x9f,0x1b) || P(0x2f,0x0b) ||
        P(0xbe,0x0a) || P(0xee,0x0a) || P(0x7e,0x0a) || P(0xeb,0x4b) ||
        P(0x3b,0x1b))
        return interp_3px(w4, 2, w3, 1, w1, 1, 2);
    return interp_3px(w4, 6, w3, 1, w1, 1, 3);
}
