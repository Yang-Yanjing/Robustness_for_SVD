static void adx_encode(ADXContext *c, uint8_t *adx, const int16_t *wav,
                       ADXChannelState *prev, int channels)
{
    PutBitContext pb;
    int scale;
    int i, j;
    int s0, s1, s2, d;
    int max = 0;
    int min = 0;
    s1 = prev->s1;
    s2 = prev->s2;
    for (i = 0, j = 0; j < 32; i += channels, j++) {
        s0 = wav[i];
        d = ((s0 << COEFF_BITS) - c->coeff[0] * s1 - c->coeff[1] * s2) >> COEFF_BITS;
        if (max < d)
            max = d;
        if (min > d)
            min = d;
        s2 = s1;
        s1 = s0;
    }
    if (max == 0 && min == 0) {
        prev->s1 = s1;
        prev->s2 = s2;
        memset(adx, 0, BLOCK_SIZE);
        return;
    }
    if (max / 7 > -min / 8)
        scale = max / 7;
    else
        scale = -min / 8;
    if (scale == 0)
        scale = 1;
    AV_WB16(adx, scale);
    init_put_bits(&pb, adx + 2, 16);
    s1 = prev->s1;
    s2 = prev->s2;
    for (i = 0, j = 0; j < 32; i += channels, j++) {
        d = ((wav[i] << COEFF_BITS) - c->coeff[0] * s1 - c->coeff[1] * s2) >> COEFF_BITS;
        d = av_clip_intp2(ROUNDED_DIV(d, scale), 3);
        put_sbits(&pb, 4, d);
        s0 = ((d << COEFF_BITS) * scale + c->coeff[0] * s1 + c->coeff[1] * s2) >> COEFF_BITS;
        s2 = s1;
        s1 = s0;
    }
    prev->s1 = s1;
    prev->s2 = s2;
    flush_put_bits(&pb);
}
