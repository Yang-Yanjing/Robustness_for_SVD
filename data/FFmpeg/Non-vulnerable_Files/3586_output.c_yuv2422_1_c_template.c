static av_always_inline void
yuv2422_1_c_template(SwsContext *c, const int16_t *buf0,
                     const int16_t *ubuf[2], const int16_t *vbuf[2],
                     const int16_t *abuf0, uint8_t *dest, int dstW,
                     int uvalpha, int y, enum AVPixelFormat target)
{
    const int16_t *ubuf0 = ubuf[0], *vbuf0 = vbuf[0];
    int i;
    if (uvalpha < 2048) {
        for (i = 0; i < ((dstW + 1) >> 1); i++) {
            int Y1 = (buf0[i * 2    ]+64) >> 7;
            int Y2 = (buf0[i * 2 + 1]+64) >> 7;
            int U  = (ubuf0[i]       +64) >> 7;
            int V  = (vbuf0[i]       +64) >> 7;
            if ((Y1 | Y2 | U | V) & 0x100) {
                Y1 = av_clip_uint8(Y1);
                Y2 = av_clip_uint8(Y2);
                U  = av_clip_uint8(U);
                V  = av_clip_uint8(V);
            }
            Y1 = av_clip_uint8(Y1);
            Y2 = av_clip_uint8(Y2);
            U  = av_clip_uint8(U);
            V  = av_clip_uint8(V);
            output_pixels(i * 4, Y1, U, Y2, V);
        }
    } else {
        const int16_t *ubuf1 = ubuf[1], *vbuf1 = vbuf[1];
        for (i = 0; i < ((dstW + 1) >> 1); i++) {
            int Y1 = (buf0[i * 2    ]    + 64) >> 7;
            int Y2 = (buf0[i * 2 + 1]    + 64) >> 7;
            int U  = (ubuf0[i] + ubuf1[i]+128) >> 8;
            int V  = (vbuf0[i] + vbuf1[i]+128) >> 8;
            if ((Y1 | Y2 | U | V) & 0x100) {
                Y1 = av_clip_uint8(Y1);
                Y2 = av_clip_uint8(Y2);
                U  = av_clip_uint8(U);
                V  = av_clip_uint8(V);
            }
            Y1 = av_clip_uint8(Y1);
            Y2 = av_clip_uint8(Y2);
            U  = av_clip_uint8(U);
            V  = av_clip_uint8(V);
            output_pixels(i * 4, Y1, U, Y2, V);
        }
    }
}
