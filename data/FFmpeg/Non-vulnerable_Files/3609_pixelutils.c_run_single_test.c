static int run_single_test(const char *test,
                           const uint8_t *block1, ptrdiff_t stride1,
                           const uint8_t *block2, ptrdiff_t stride2,
                           int align, int n)
{
    int out, ref;
    av_pixelutils_sad_fn f_ref = sad_c[n - 1];
    av_pixelutils_sad_fn f_out = av_pixelutils_get_sad_fn(n, n, align, NULL);
    switch (align) {
    case 0: block1++; block2++; break;
    case 1:           block2++; break;
    case 2:                     break;
    }
    out = f_out(block1, stride1, block2, stride2);
    ref = f_ref(block1, stride1, block2, stride2);
    printf("[%s] [%c%c] SAD [%s] %dx%d=%d ref=%d\n",
           out == ref ? "OK" : "FAIL",
           align ? 'A' : 'U', align == 2 ? 'A' : 'U',
           test, 1<<n, 1<<n, out, ref);
    return out != ref;
}
