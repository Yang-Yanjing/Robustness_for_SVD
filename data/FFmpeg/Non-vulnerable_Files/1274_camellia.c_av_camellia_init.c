av_cold int av_camellia_init(AVCAMELLIA *cs, const uint8_t *key, int key_bits)
{
    uint64_t Kl[2], Kr[2], Ka[2], Kb[2];
    uint64_t D1, D2;
    if (key_bits != 128 && key_bits != 192 && key_bits != 256)
        return -1;
    memset(Kb, 0, sizeof(Kb));
    memset(Kr, 0, sizeof(Kr));
    cs->key_bits = key_bits;
    Kl[0] = AV_RB64(key);
    Kl[1] = AV_RB64(key + 8);
    if (key_bits == 192) {
        Kr[0] = AV_RB64(key + 16);
        Kr[1] = ~Kr[0];
    } else if (key_bits == 256) {
        Kr[0] = AV_RB64(key + 16);
        Kr[1] = AV_RB64(key + 24);
    }
    computeSP();
    D1 = Kl[0] ^ Kr[0];
    D2 = Kl[1] ^ Kr[1];
    D2 ^= F(D1, Sigma1);
    D1 ^= F(D2, Sigma2);
    D1 ^= Kl[0];
    D2 ^= Kl[1];
    D2 ^= F(D1, Sigma3);
    D1 ^= F(D2, Sigma4);
    Ka[0] = D1;
    Ka[1] = D2;
    if (key_bits != 128) {
        D1 = Ka[0] ^ Kr[0];
        D2 = Ka[1] ^ Kr[1];
        D2 ^= F(D1, Sigma5);
        D1 ^= F(D2, Sigma6);
        Kb[0] = D1;
        Kb[1] = D2;
    }
    generate_round_keys(cs, Kl, Kr, Ka, Kb);
    return 0;
}
