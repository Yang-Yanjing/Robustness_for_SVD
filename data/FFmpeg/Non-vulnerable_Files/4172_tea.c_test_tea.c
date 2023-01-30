static void test_tea(AVTEA *ctx, uint8_t *dst, const uint8_t *src,
                     const uint8_t *ref, int len, uint8_t *iv, int dir,
                     const char *test)
{
    av_tea_crypt(ctx, dst, src, len, iv, dir);
    if (memcmp(dst, ref, 8*len)) {
        int i;
        printf("%s failed\ngot      ", test);
        for (i = 0; i < 8*len; i++)
            printf("%02x ", dst[i]);
        printf("\nexpected ");
        for (i = 0; i < 8*len; i++)
            printf("%02x ", ref[i]);
        printf("\n");
        exit(1);
    }
}
