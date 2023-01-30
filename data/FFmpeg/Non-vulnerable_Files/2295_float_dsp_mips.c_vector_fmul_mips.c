static void vector_fmul_mips(float *dst, const float *src0, const float *src1,
                             int len)
{
    int i;
    if (len & 3) {
        for (i = 0; i < len; i++)
            dst[i] = src0[i] * src1[i];
    } else {
        float *d     = (float *)dst;
        float *d_end = d + len;
        float *s0    = (float *)src0;
        float *s1    = (float *)src1;
        float src0_0, src0_1, src0_2, src0_3;
        float src1_0, src1_1, src1_2, src1_3;
        __asm__ volatile (
            "1:                                         \n\t"
            "lwc1   %[src0_0],  0(%[s0])                \n\t"
            "lwc1   %[src1_0],  0(%[s1])                \n\t"
            "lwc1   %[src0_1],  4(%[s0])                \n\t"
            "lwc1   %[src1_1],  4(%[s1])                \n\t"
            "lwc1   %[src0_2],  8(%[s0])                \n\t"
            "lwc1   %[src1_2],  8(%[s1])                \n\t"
            "lwc1   %[src0_3],  12(%[s0])               \n\t"
            "lwc1   %[src1_3],  12(%[s1])               \n\t"
            "mul.s  %[src0_0],  %[src0_0],  %[src1_0]   \n\t"
            "mul.s  %[src0_1],  %[src0_1],  %[src1_1]   \n\t"
            "mul.s  %[src0_2],  %[src0_2],  %[src1_2]   \n\t"
            "mul.s  %[src0_3],  %[src0_3],  %[src1_3]   \n\t"
            "swc1   %[src0_0],  0(%[d])                 \n\t"
            "swc1   %[src0_1],  4(%[d])                 \n\t"
            "swc1   %[src0_2],  8(%[d])                 \n\t"
            "swc1   %[src0_3],  12(%[d])                \n\t"
            PTR_ADDIU "%[s0],   %[s0],      16          \n\t"
            PTR_ADDIU "%[s1],   %[s1],      16          \n\t"
            PTR_ADDIU "%[d],    %[d],       16          \n\t"
            "bne    %[d],       %[d_end],   1b          \n\t"
            : [src0_0]"=&f"(src0_0), [src0_1]"=&f"(src0_1),
              [src0_2]"=&f"(src0_2), [src0_3]"=&f"(src0_3),
              [src1_0]"=&f"(src1_0), [src1_1]"=&f"(src1_1),
              [src1_2]"=&f"(src1_2), [src1_3]"=&f"(src1_3),
              [d]"+r"(d), [s0]"+r"(s0), [s1]"+r"(s1)
            : [d_end]"r"(d_end)
            : "memory"
        );
    }
}
