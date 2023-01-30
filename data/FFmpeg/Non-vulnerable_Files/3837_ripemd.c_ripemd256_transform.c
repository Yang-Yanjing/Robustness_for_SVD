static void ripemd256_transform(uint32_t *state, const uint8_t buffer[64])
{
    uint32_t a, b, c, d, e, f, g, h, av_unused t;
    uint32_t block[16];
    int n;
    a = state[0]; b = state[1]; c = state[2]; d = state[3];
    e = state[4]; f = state[5]; g = state[6]; h = state[7];
    for (n = 0; n < 16; n++)
        block[n] = AV_RL32(buffer + 4 * n);
    n = 0;
#if CONFIG_SMALL
    for (; n < 16;) {
        ROUND128_0_TO_15(a,b,c,d,e,f,g,h);
        t = d; d = c; c = b; b = a; a = t;
        t = h; h = g; g = f; f = e; e = t;
    }
    FFSWAP(uint32_t, a, e);
    for (; n < 32;) {
        ROUND128_16_TO_31(a,b,c,d,e,f,g,h);
        t = d; d = c; c = b; b = a; a = t;
        t = h; h = g; g = f; f = e; e = t;
    }
    FFSWAP(uint32_t, b, f);
    for (; n < 48;) {
        ROUND128_32_TO_47(a,b,c,d,e,f,g,h);
        t = d; d = c; c = b; b = a; a = t;
        t = h; h = g; g = f; f = e; e = t;
    }
    FFSWAP(uint32_t, c, g);
    for (; n < 64;) {
        ROUND128_48_TO_63(a,b,c,d,e,f,g,h);
        t = d; d = c; c = b; b = a; a = t;
        t = h; h = g; g = f; f = e; e = t;
    }
    FFSWAP(uint32_t, d, h);
#else
    R128_0; R128_0; R128_0; R128_0;
    FFSWAP(uint32_t, a, e);
    R128_16; R128_16; R128_16; R128_16;
    FFSWAP(uint32_t, b, f);
    R128_32; R128_32; R128_32; R128_32;
    FFSWAP(uint32_t, c, g);
    R128_48; R128_48; R128_48; R128_48;
    FFSWAP(uint32_t, d, h);
#endif
    state[0] += a; state[1] += b; state[2] += c; state[3] += d;
    state[4] += e; state[5] += f; state[6] += g; state[7] += h;
}
