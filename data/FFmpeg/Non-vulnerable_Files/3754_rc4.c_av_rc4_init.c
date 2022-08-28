int av_rc4_init(AVRC4 *r, const uint8_t *key, int key_bits, int decrypt) {
    int i, j;
    uint8_t y;
    uint8_t *state = r->state;
    int keylen = key_bits >> 3;
    if (key_bits & 7)
        return -1;
    for (i = 0; i < 256; i++)
        state[i] = i;
    y = 0;
    
    for (j = 0, i = 0; i < 256; i++, j++) {
        if (j == keylen) j = 0;
        y += state[i] + key[j];
        FFSWAP(uint8_t, state[i], state[y]);
    }
    r->x = 1;
    r->y = state[1];
    return 0;
}
