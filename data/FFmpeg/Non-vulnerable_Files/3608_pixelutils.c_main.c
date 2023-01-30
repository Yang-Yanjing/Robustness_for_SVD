int main(void)
{
    int i, align, ret;
    uint8_t *buf1 = av_malloc(W1*H1);
    uint8_t *buf2 = av_malloc(W2*H2);
    uint32_t state = 0;
    if (!buf1 || !buf2) {
        fprintf(stderr, "malloc failure\n");
        ret = 1;
        goto end;
    }
    ff_check_pixfmt_descriptors();
#define RANDOM_INIT(buf, size) do {             \
    int k;                                      \
    for (k = 0; k < size; k++) {                \
        state = state * 1664525 + 1013904223;   \
        buf[k] = state>>24;                     \
    }                                           \
} while (0)
    
    RANDOM_INIT(buf1, W1*H1);
    RANDOM_INIT(buf2, W2*H2);
    ret = run_test("random", buf1, buf2);
    if (ret < 0)
        goto end;
    
    memset(buf1, 0xff, W1*H1);
    memset(buf2, 0x00, W2*H2);
    ret = run_test("max", buf1, buf2);
    if (ret < 0)
        goto end;
    
    memset(buf1, 0x90, W1*H1);
    memset(buf2, 0x90, W2*H2);
    ret = run_test("min", buf1, buf2);
    if (ret < 0)
        goto end;
    
    for (i = 1; i <= 4; i++) {
        for (align = 0; align < 3; align++) {
            int size1, size2;
            av_freep(&buf1);
            av_freep(&buf2);
            size1 = size2 = 1 << (i << 1);
            switch (align) {
            case 0: size1++; size2++; break;
            case 1:          size2++; break;
            case 2:                   break;
            }
            buf1 = av_malloc(size1);
            buf2 = av_malloc(size2);
            if (!buf1 || !buf2) {
                fprintf(stderr, "malloc failure\n");
                ret = 1;
                goto end;
            }
            RANDOM_INIT(buf1, size1);
            RANDOM_INIT(buf2, size2);
            ret = run_single_test("small", buf1, 1<<i, buf2, 1<<i, align, i);
            if (ret < 0)
                goto end;
        }
    }
end:
    av_free(buf1);
    av_free(buf2);
    return ret;
}
