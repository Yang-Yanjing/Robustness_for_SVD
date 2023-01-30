static void twofish_decrypt(AVTWOFISH *cs, uint8_t *dst, const uint8_t *src, uint8_t *iv)
{
    uint32_t P[4], t0, t1;
    int i;
    P[2] = AV_RL32(src) ^ cs->K[4];
    P[3] = AV_RL32(src + 4) ^ cs->K[5];
    P[0] = AV_RL32(src + 8) ^ cs->K[6];
    P[1] = AV_RL32(src + 12) ^ cs->K[7];
    for (i = 15; i >= 0; i -= 2) {
        t0 = MDS_mul(cs, P[2]);
        t1 = MDS_mul(cs, LR(P[3], 8));
        P[0] = LR(P[0], 1) ^ (t0 + t1 + cs->K[2 * i + 8]);
        P[1] = RR(P[1] ^ (t0 + 2 * t1 + cs->K[2 * i + 9]), 1);
        t0 = MDS_mul(cs, P[0]);
        t1 = MDS_mul(cs, LR(P[1], 8));
        P[2] = LR(P[2], 1) ^ (t0 + t1 + cs->K[2 * i + 6]);
        P[3] = RR(P[3] ^ (t0 + 2 * t1 + cs->K[2 * i + 7]), 1);
    }
    P[0] ^= cs->K[0];
    P[1] ^= cs->K[1];
    P[2] ^= cs->K[2];
    P[3] ^= cs->K[3];
    if (iv) {
        P[0] ^= AV_RL32(iv);
        P[1] ^= AV_RL32(iv + 4);
        P[2] ^= AV_RL32(iv + 8);
        P[3] ^= AV_RL32(iv + 12);
        memcpy(iv, src, 16);
    }
    AV_WL32(dst, P[2]);
    AV_WL32(dst + 4, P[3]);
    AV_WL32(dst + 8, P[0]);
    AV_WL32(dst + 12, P[1]);
}
