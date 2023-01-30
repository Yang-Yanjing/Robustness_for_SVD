static void
yuv2ya8_1_c(SwsContext *c, const int16_t *buf0,
            const int16_t *ubuf[2], const int16_t *vbuf[2],
            const int16_t *abuf0, uint8_t *dest, int dstW,
            int uvalpha, int y)
{
    int hasAlpha = !!abuf0;
    int i;
    for (i = 0; i < dstW; i++) {
        int Y = (buf0[i] + 64) >> 7;
        int A;
        Y = av_clip_uint8(Y);
        if (hasAlpha) {
            A = (abuf0[i] + 64) >> 7;
            if (A & 0x100)
                A = av_clip_uint8(A);
        }
        dest[i * 2    ] = Y;
        dest[i * 2 + 1] = hasAlpha ? A : 255;
    }
}
