static void vector_fmul_window_scaled_c(int16_t *dst, const int32_t *src0,
                                       const int32_t *src1, const int32_t *win,
                                       int len, uint8_t bits)
{
    int32_t s0, s1, wi, wj, i,j, round;
    dst += len;
    win += len;
    src0+= len;
    round = bits? 1 << (bits-1) : 0;
    for (i=-len, j=len-1; i<0; i++, j--) {
        s0 = src0[i];
        s1 = src1[j];
        wi = win[i];
        wj = win[j];
        dst[i] = av_clip_int16(((((int64_t)s0*wj - (int64_t)s1*wi + 0x40000000) >> 31) + round) >> bits);
        dst[j] = av_clip_int16(((((int64_t)s0*wi + (int64_t)s1*wj + 0x40000000) >> 31) + round) >> bits);
    }
}
