int main(void)
{
    AVXTEA ctx;
    uint8_t buf[8], iv[8];
    int i;
    static const uint8_t src[32] = "HelloWorldHelloWorldHelloWorld";
    uint8_t ct[32];
    uint8_t pl[32];
    for (i = 0; i < XTEA_NUM_TESTS; i++) {
        av_xtea_init(&ctx, xtea_test_key[i]);
        test_xtea(&ctx, buf, xtea_test_pt[i], xtea_test_ct[i], 1, NULL, 0, "encryption");
        test_xtea(&ctx, buf, xtea_test_ct[i], xtea_test_pt[i], 1, NULL, 1, "decryption");
        
        memcpy(iv, "HALLO123", 8);
        av_xtea_crypt(&ctx, ct, src, 4, iv, 0);
        
        memcpy(iv, "HALLO123", 8);
        test_xtea(&ctx, pl, ct, src, 4, iv, 1, "CBC decryption");
        memcpy(iv, "HALLO123", 8);
        test_xtea(&ctx, ct, ct, src, 4, iv, 1, "CBC inplace decryption");
    }
    printf("Test encryption/decryption success.\n");
    return 0;
}
