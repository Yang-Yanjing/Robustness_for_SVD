void av_rc4_crypt(AVRC4 *r, uint8_t *dst, const uint8_t *src, int count, uint8_t *iv, int decrypt) {
    uint8_t x = r->x, y = r->y;
    uint8_t *state = r->state;
    while (count-- > 0) {
        uint8_t sum = state[x] + state[y];
        FFSWAP(uint8_t, state[x], state[y]);
        *dst++ = src ? *src++ ^ state[sum] : state[sum];
        x++;
        y += state[x];
    }
    r->x = x; r->y = y;
}
