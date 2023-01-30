    );
static void ac3_downmix_sse(float **samples, float (*matrix)[2],
                            int out_ch, int in_ch, int len)
{
    int (*matrix_cmp)[2] = (int(*)[2])matrix;
    intptr_t i, j, k, m;
    i = -len * sizeof(float);
    if (in_ch == 5 && out_ch == 2 &&
        !(matrix_cmp[0][1] | matrix_cmp[2][0]   |
          matrix_cmp[3][1] | matrix_cmp[4][0]   |
          (matrix_cmp[1][0] ^ matrix_cmp[1][1]) |
          (matrix_cmp[0][0] ^ matrix_cmp[2][1]))) {
        MIX5(IF0, IF1);
    } else if (in_ch == 5 && out_ch == 1 &&
               matrix_cmp[0][0] == matrix_cmp[2][0] &&
               matrix_cmp[3][0] == matrix_cmp[4][0]) {
        MIX5(IF1, IF0);
    } else {
        LOCAL_ALIGNED(16, float, matrix_simd, [AC3_MAX_CHANNELS], [2][4]);
        float *samp[AC3_MAX_CHANNELS];
        for (j = 0; j < in_ch; j++)
            samp[j] = samples[j] + len;
        j = 2 * in_ch * sizeof(float);
        __asm__ volatile (
            "1:                                 \n"
            "sub             $8, %0             \n"
            "movss     (%2, %0), %%xmm4         \n"
            "movss    4(%2, %0), %%xmm5         \n"
            "shufps          $0, %%xmm4, %%xmm4 \n"
            "shufps          $0, %%xmm5, %%xmm5 \n"
            "movaps      %%xmm4,   (%1, %0, 4)  \n"
            "movaps      %%xmm5, 16(%1, %0, 4)  \n"
            "jg              1b                 \n"
            : "+&r"(j)
            : "r"(matrix_simd), "r"(matrix)
            : "memory"
        );
        if (out_ch == 2) {
            MIX_MISC(IF1);
        } else {
            MIX_MISC(IF0);
        }
    }
}
