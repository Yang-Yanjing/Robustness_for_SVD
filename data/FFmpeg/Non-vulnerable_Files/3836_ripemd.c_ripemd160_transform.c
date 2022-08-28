    ROUND160_64_TO_79(c,d,e,a,b,h,i,j,f,g)
static void ripemd160_transform(uint32_t *state, const uint8_t buffer[64])
{
    uint32_t a, b, c, d, e, f, g, h, i, j, av_unused t;
    uint32_t block[16];
    int n;
    a = f = state[0];
    b = g = state[1];
    c = h = state[2];
    d = i = state[3];
    e = j = state[4];
    for (n = 0; n < 16; n++)
        block[n] = AV_RL32(buffer + 4 * n);
    n = 0;
#if CONFIG_SMALL
    for (; n < 16;) {
        ROUND160_0_TO_15(a,b,c,d,e,f,g,h,i,j);
        t = e; e = d; d = c; c = b; b = a; a = t;
        t = j; j = i; i = h; h = g; g = f; f = t;
    }
    for (; n < 32;) {
        ROUND160_16_TO_31(a,b,c,d,e,f,g,h,i,j);
        t = e; e = d; d = c; c = b; b = a; a = t;
        t = j; j = i; i = h; h = g; g = f; f = t;
    }
    for (; n < 48;) {
        ROUND160_32_TO_47(a,b,c,d,e,f,g,h,i,j);
        t = e; e = d; d = c; c = b; b = a; a = t;
        t = j; j = i; i = h; h = g; g = f; f = t;
    }
    for (; n < 64;) {
        ROUND160_48_TO_63(a,b,c,d,e,f,g,h,i,j);
        t = e; e = d; d = c; c = b; b = a; a = t;
        t = j; j = i; i = h; h = g; g = f; f = t;
    }
    for (; n < 80;) {
        ROUND160_64_TO_79(a,b,c,d,e,f,g,h,i,j);
        t = e; e = d; d = c; c = b; b = a; a = t;
        t = j; j = i; i = h; h = g; g = f; f = t;
    }
#else
    R160_0; R160_0; R160_0;
    ROUND160_0_TO_15(a,b,c,d,e,f,g,h,i,j);
    R160_16; R160_16; R160_16;
    ROUND160_16_TO_31(e,a,b,c,d,j,f,g,h,i);
    R160_32; R160_32; R160_32;
    ROUND160_32_TO_47(d,e,a,b,c,i,j,f,g,h);
    R160_48; R160_48; R160_48;
    ROUND160_48_TO_63(c,d,e,a,b,h,i,j,f,g);
    R160_64; R160_64; R160_64;
    ROUND160_64_TO_79(b,c,d,e,a,g,h,i,j,f);
#endif
    i += c + state[1];
    state[1] = state[2] + d + j;
    state[2] = state[3] + e + f;
    state[3] = state[4] + a + g;
    state[4] = state[0] + b + h;
    state[0] = i;
}
