av_cold int av_twofish_init(AVTWOFISH *cs, const uint8_t *key, int key_bits)
{
    int i;
    uint8_t keypad[32];
    uint32_t Key[8], Me[4], Mo[4], A, B;
    const uint32_t rho = 0x01010101;
    if (key_bits < 0)
        return -1;
    if (key_bits <= 128) {
        cs->ksize = 2;
    } else if (key_bits <= 192) {
        cs->ksize = 3;
    } else {
        cs->ksize = 4;
    }
    memset(keypad, 0, sizeof(keypad));
    if (key_bits <= 256) {
        memcpy(keypad, key, key_bits >> 3);
    } else {
        memcpy(keypad, key, 32);
    }
    for (i = 0; i < 2 * cs->ksize ; i++)
        Key[i] = AV_RL32(keypad + 4 * i);
    for (i = 0; i < cs->ksize; i++) {
        Me[i] = Key[2 * i];
        Mo[i] = Key[2 * i + 1];
        cs->S[cs->ksize - i - 1] = tf_RS(Me[i], Mo[i]);
    }
    precomputeMDS(cs);
    for (i = 0; i < 20; i++) {
        A = tf_h((2 * i) * rho, Me, cs->ksize);
        B = tf_h((2 * i + 1) * rho, Mo, cs->ksize);
        B = LR(B, 8);
        cs->K[2 * i] = A + B;
        cs->K[2 * i + 1] = LR((A + (2 * B)), 9);
    }
    if (cs->ksize << 6 != key_bits) {
        return 1;
    } else {
        return 0;
    }
}
