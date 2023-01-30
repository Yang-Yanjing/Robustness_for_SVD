static void camellia_encrypt(AVCAMELLIA *cs, uint8_t *dst, const uint8_t *src)
{
    uint64_t D1, D2;
    D1 = AV_RB64(src);
    D2 = AV_RB64(src + 8);
    D1 ^= cs->Kw[0];
    D2 ^= cs->Kw[1];
    D2 ^= F(D1, cs->K[0]);
    D1 ^= F(D2, cs->K[1]);
    D2 ^= F(D1, cs->K[2]);
    D1 ^= F(D2, cs->K[3]);
    D2 ^= F(D1, cs->K[4]);
    D1 ^= F(D2, cs->K[5]);
    D1 = FL(D1, cs->Ke[0]);
    D2 = FLINV(D2, cs->Ke[1]);
    D2 ^= F(D1, cs->K[6]);
    D1 ^= F(D2, cs->K[7]);
    D2 ^= F(D1, cs->K[8]);
    D1 ^= F(D2, cs->K[9]);
    D2 ^= F(D1, cs->K[10]);
    D1 ^= F(D2, cs->K[11]);
    D1 = FL(D1, cs->Ke[2]);
    D2 = FLINV(D2, cs->Ke[3]);
    D2 ^= F(D1, cs->K[12]);
    D1 ^= F(D2, cs->K[13]);
    D2 ^= F(D1, cs->K[14]);
    D1 ^= F(D2, cs->K[15]);
    D2 ^= F(D1, cs->K[16]);
    D1 ^= F(D2, cs->K[17]);
    if (cs->key_bits != 128) {
        D1 = FL(D1, cs->Ke[4]);
        D2 = FLINV(D2, cs->Ke[5]);
        D2 ^= F(D1, cs->K[18]);
        D1 ^= F(D2, cs->K[19]);
        D2 ^= F(D1, cs->K[20]);
        D1 ^= F(D2, cs->K[21]);
        D2 ^= F(D1, cs->K[22]);
        D1 ^= F(D2, cs->K[23]);
    }
    D2 ^= cs->Kw[2];
    D1 ^= cs->Kw[3];
    AV_WB64(dst, D2);
    AV_WB64(dst + 8, D1);
}
