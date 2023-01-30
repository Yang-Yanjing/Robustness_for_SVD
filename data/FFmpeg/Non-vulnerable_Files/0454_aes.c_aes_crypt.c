static inline void aes_crypt(AVAES *a, int s, const uint8_t *sbox,
                         uint32_t multbl[][256])
{
    int r;
    for (r = a->rounds - 1; r > 0; r--) {
        mix(a->state, multbl, 3 - s, 1 + s);
        addkey(&a->state[1], &a->state[0], &a->round_key[r]);
    }
    subshift(&a->state[0], s, sbox);
}
