int main(void) {
    AVDES d;
    int i;
#ifdef GENTABLES
    int j;
#endif
    uint64_t key[3];
    uint64_t data;
    uint64_t ct;
    uint64_t roundkeys[16];
    srand(av_gettime());
    key[0] = AV_RB64(test_key);
    data = AV_RB64(plain);
    gen_roundkeys(roundkeys, key[0]);
    if (des_encdec(data, roundkeys, 0) != AV_RB64(crypt)) {
        printf("Test 1 failed\n");
        return 1;
    }
    av_des_init(&d, test_key, 64, 0);
    av_des_crypt(&d, tmp, plain, 1, NULL, 0);
    if (memcmp(tmp, crypt, sizeof(crypt))) {
        printf("Public API decryption failed\n");
        return 1;
    }
    if (!run_test(0, 0) || !run_test(0, 1) || !run_test(1, 0) || !run_test(1, 1)) {
        printf("Partial Monte-Carlo test failed\n");
        return 1;
    }
    for (i = 0; i < 1000; i++) {
        key[0] = rand64(); key[1] = rand64(); key[2] = rand64();
        data = rand64();
        av_des_init(&d, (uint8_t*)key, 192, 0);
        av_des_crypt(&d, (uint8_t*)&ct, (uint8_t*)&data, 1, NULL, 0);
        av_des_init(&d, (uint8_t*)key, 192, 1);
        av_des_crypt(&d, (uint8_t*)&ct, (uint8_t*)&ct, 1, NULL, 1);
        if (ct != data) {
            printf("Test 2 failed\n");
            return 1;
        }
    }
#ifdef GENTABLES
    printf("static const uint32_t S_boxes_P_shuffle[8][64] = {\n");
    for (i = 0; i < 8; i++) {
        printf("    {");
        for (j = 0; j < 64; j++) {
            uint32_t v = S_boxes[i][j >> 1];
            v = j & 1 ? v >> 4 : v & 0xf;
            v <<= 28 - 4 * i;
            v = shuffle(v, P_shuffle, sizeof(P_shuffle));
            printf((j & 7) == 0 ? "\n    " : " ");
            printf("0x%08X,", v);
        }
        printf("\n    },\n");
    }
    printf("};\n");
#endif
    return 0;
}
