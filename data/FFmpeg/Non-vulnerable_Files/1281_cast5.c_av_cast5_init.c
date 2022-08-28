av_cold int av_cast5_init(AVCAST5* cs, const uint8_t *key, int key_bits)
{
    uint8_t newKey[16];
    int i;
    uint32_t p[4], q[4];
    if (key_bits % 8 || key_bits < 40 || key_bits > 128)
        return -1;
    memset(newKey, 0, sizeof(newKey));
    memcpy(newKey, key, key_bits >> 3);
    cs->rounds = key_bits <= 80 ? 12 : 16;
    for (i = 0; i < 4; i++)
        q[i] = AV_RB32(newKey + (4 * i));
    generate_round_keys(cs->rounds, cs->Km, q, p);
    generate_round_keys(cs->rounds, cs->Kr, q, p);
    for (i = 0; i <= cs->rounds; i++)
        cs->Kr[i] = cs->Kr[i] & 0x1f;
    return 0;
}
