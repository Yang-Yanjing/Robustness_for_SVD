static void ripemd320_transform(uint32_t *state, const uint8_t buffer[64])
{
    uint32_t a, b, c, d, e, f, g, h, i, j, av_unused t;
    uint32_t block[16];
    int n;
    a = state[0]; b = state[1]; c = state[2]; d = state[3]; e = state[4];
    f = state[5]; g = state[6]; h = state[7]; i = state[8]; j = state[9];
    for (n = 0; n < 16; n++)
        block[n] = AV_RL32(buffer + 4 * n);
    n = 0;
#if CONFIG_SMALL
    for (; n < 16;) {
        ROUND160_0_TO_15(a,b,c,d,e,f,g,h,i,j);
        t = e; e = d; d = c; c = b; b = a; a = t;
        t = j; j = i; i = h; h = g; g = f; f = t;
    }
    FFSWAP(uint32_t, b, g);
    for (; n < 32;) {
        ROUND160_16_TO_31(a,b,c,d,e,f,g,h,i,j);
        t = e; e = d; d = c; c = b; b = a; a = t;
        t = j; j = i; i = h; h = g; g = f; f = t;
    }
    FFSWAP(uint32_t, d, i);
    for (; n < 48;) {
        ROUND160_32_TO_47(a,b,c,d,e,f,g,h,i,j);
        t = e; e = d; d = c; c = b; b = a; a = t;
        t = j; j = i; i = h; h = g; g = f; f = t;
    }
    FFSWAP(uint32_t, a, f);
    for (; n < 64;) {
        ROUND160_48_TO_63(a,b,c,d,e,f,g,h,i,j);
        t = e; e = d; d = c; c = b; b = a; a = t;
        t = j; j = i; i = h; h = g; g = f; f = t;
    }
    FFSWAP(uint32_t, c, h);
    for (; n < 80;) {
        ROUND160_64_TO_79(a,b,c,d,e,f,g,h,i,j);
        t = e; e = d; d = c; c = b; b = a; a = t;
        t = j; j = i; i = h; h = g; g = f; f = t;
    }
    FFSWAP(uint32_t, e, j);
#else
    R160_0; R160_0; R160_0;
    ROUND160_0_TO_15(a,b,c,d,e,f,g,h,i,j);
    FFSWAP(uint32_t, a, f);
    R160_16; R160_16; R160_16;
    ROUND160_16_TO_31(e,a,b,c,d,j,f,g,h,i);
    FFSWAP(uint32_t, b, g);
    R160_32; R160_32; R160_32;
    ROUND160_32_TO_47(d,e,a,b,c,i,j,f,g,h);
    FFSWAP(uint32_t, c, h);
    R160_48; R160_48; R160_48;
    ROUND160_48_TO_63(c,d,e,a,b,h,i,j,f,g);
    FFSWAP(uint32_t, d, i);
    R160_64; R160_64; R160_64;
    ROUND160_64_TO_79(b,c,d,e,a,g,h,i,j,f);
    FFSWAP(uint32_t, e, j);
#endif
    state[0] += a; state[1] += b; state[2] += c; state[3] += d; state[4] += e;
    state[5] += f; state[6] += g; state[7] += h; state[8] += i; state[9] += j;
}
