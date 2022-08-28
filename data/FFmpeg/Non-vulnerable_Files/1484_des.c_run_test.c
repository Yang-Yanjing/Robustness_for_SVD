static int run_test(int cbc, int decrypt) {
    AVDES d;
    int delay = cbc && !decrypt ? 2 : 1;
    uint64_t res;
    AV_WB64(large_buffer[0], 0x4e6f772069732074ULL);
    AV_WB64(large_buffer[1], 0x1234567890abcdefULL);
    AV_WB64(tmp,             0x1234567890abcdefULL);
    av_des_init(&d, cbc_key, 192, decrypt);
    av_des_crypt(&d, large_buffer[delay], large_buffer[0], 10000, cbc ? tmp : NULL, decrypt);
    res = AV_RB64(large_buffer[9999 + delay]);
    if (cbc) {
        if (decrypt)
            return res == 0xc5cecf63ecec514cULL;
        else
            return res == 0xcb191f85d1ed8439ULL;
    } else {
        if (decrypt)
            return res == 0x8325397644091a0aULL;
        else
            return res == 0xdd17e8b8b437d232ULL;
    }
}
