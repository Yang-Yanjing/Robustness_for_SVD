    ROUND128_48_TO_63(b,c,d,a,f,g,h,e)
static void ripemd128_transform(uint32_t *state, const uint8_t buffer[64])
{
    uint32_t a, b, c, d, e, f, g, h, av_unused t;
    uint32_t block[16];
    int n;
    a = e = state[0];
    b = f = state[1];
    c = g = state[2];
    d = h = state[3];
    for (n = 0; n < 16; n++)
        block[n] = AV_RL32(buffer + 4 * n);
    n = 0;
#if CONFIG_SMALL
    for (; n < 16;) {
        ROUND128_0_TO_15(a,b,c,d,e,f,g,h);
        t = d; d = c; c = b; b = a; a = t;
        t = h; h = g; g = f; f = e; e = t;
    }
    for (; n < 32;) {
        ROUND128_16_TO_31(a,b,c,d,e,f,g,h);
        t = d; d = c; c = b; b = a; a = t;
        t = h; h = g; g = f; f = e; e = t;
    }
    for (; n < 48;) {
        ROUND128_32_TO_47(a,b,c,d,e,f,g,h);
        t = d; d = c; c = b; b = a; a = t;
        t = h; h = g; g = f; f = e; e = t;
    }
    for (; n < 64;) {
        ROUND128_48_TO_63(a,b,c,d,e,f,g,h);
        t = d; d = c; c = b; b = a; a = t;
        t = h; h = g; g = f; f = e; e = t;
    }
#else
    R128_0; R128_0; R128_0; R128_0;
    R128_16; R128_16; R128_16; R128_16;
    R128_32; R128_32; R128_32; R128_32;
    R128_48; R128_48; R128_48; R128_48;
#endif
    h += c + state[1];
    state[1] = state[2] + d + e;
    state[2] = state[3] + a + f;
    state[3] = state[0] + b + g;
    state[0] = h;
}
