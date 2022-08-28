int main(void)
{
    AVTEA *ctx;
    uint8_t buf[8], iv[8];
    int i;
    static const uint8_t src[32] = "HelloWorldHelloWorldHelloWorld";
    uint8_t ct[32];
    uint8_t pl[32];
    ctx = av_tea_alloc();
    if (!ctx)
        return 1;
    for (i = 0; i < TEA_NUM_TESTS; i++) {
        av_tea_init(ctx, tea_test_key[i], 64);
        test_tea(ctx, buf, tea_test_pt[i], tea_test_ct[i], 1, NULL, 0, "encryption");
        test_tea(ctx, buf, tea_test_ct[i], tea_test_pt[i], 1, NULL, 1, "decryption");
        
        memcpy(iv, "HALLO123", 8);
        av_tea_crypt(ctx, ct, src, 4, iv, 0);
        
        memcpy(iv, "HALLO123", 8);
        test_tea(ctx, pl, ct, src, 4, iv, 1, "CBC decryption");
        memcpy(iv, "HALLO123", 8);
        test_tea(ctx, ct, ct, src, 4, iv, 1, "CBC inplace decryption");
    }
    printf("Test encryption/decryption success.\n");
    av_free(ctx);
    return 0;
}
