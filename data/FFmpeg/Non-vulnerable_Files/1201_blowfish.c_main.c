int main(void)
{
    AVBlowfish ctx;
    uint32_t tmptext_l[NUM_VARIABLE_KEY_TESTS];
    uint32_t tmptext_r[NUM_VARIABLE_KEY_TESTS];
    uint8_t tmp[16], iv[8];
    int i;
    av_blowfish_init(&ctx, "abcdefghijklmnopqrstuvwxyz", 26);
    test_blowfish(&ctx, tmp, plaintext, ciphertext, 1, NULL, 0, "encryption");
    test_blowfish(&ctx, tmp, ciphertext, plaintext, 1, NULL, 1, "decryption");
    test_blowfish(&ctx, tmp, tmp, ciphertext, 1, NULL, 0, "Inplace encryption");
    test_blowfish(&ctx, tmp, tmp, plaintext,  1, NULL, 1, "Inplace decryption");
    memcpy(iv, IV, 8);
    test_blowfish(&ctx, tmp, plaintext2, ciphertext2, 2, iv, 0, "CBC encryption");
    memcpy(iv, IV, 8);
    test_blowfish(&ctx, tmp, ciphertext2, plaintext2, 2, iv, 1, "CBC decryption");
    memcpy(iv, IV, 8);
    test_blowfish(&ctx, tmp, tmp, ciphertext2, 2, iv, 0, "Inplace CBC encryption");
    memcpy(iv, IV, 8);
    test_blowfish(&ctx, tmp, tmp, plaintext2,  2, iv, 1, "Inplace CBC decryption");
    memcpy(tmptext_l, plaintext_l, sizeof(*plaintext_l) * NUM_VARIABLE_KEY_TESTS);
    memcpy(tmptext_r, plaintext_r, sizeof(*plaintext_r) * NUM_VARIABLE_KEY_TESTS);
    for (i = 0; i < NUM_VARIABLE_KEY_TESTS; i++) {
        av_blowfish_init(&ctx, variable_key[i], 8);
        av_blowfish_crypt_ecb(&ctx, &tmptext_l[i], &tmptext_r[i], 0);
        if (tmptext_l[i] != ciphertext_l[i] || tmptext_r[i] != ciphertext_r[i]) {
            printf("Test encryption failed.\n");
            return 1;
        }
        av_blowfish_crypt_ecb(&ctx, &tmptext_l[i], &tmptext_r[i], 1);
        if (tmptext_l[i] != plaintext_l[i] || tmptext_r[i] != plaintext_r[i]) {
            printf("Test decryption failed.\n");
            return 1;
        }
    }
    printf("Test encryption/decryption success.\n");
    return 0;
}
