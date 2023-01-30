static void float_to_fixed24_mips(int32_t *dst, const float *src, unsigned int len)
{
    const float scale = 1 << 24;
    float src0, src1, src2, src3, src4, src5, src6, src7;
    int temp0, temp1, temp2, temp3, temp4, temp5, temp6, temp7;
    do {
        __asm__ volatile (
            "lwc1       %[src0],    0(%[src])               \n\t"
            "lwc1       %[src1],    4(%[src])               \n\t"
            "lwc1       %[src2],    8(%[src])               \n\t"
            "lwc1       %[src3],    12(%[src])              \n\t"
            "lwc1       %[src4],    16(%[src])              \n\t"
            "lwc1       %[src5],    20(%[src])              \n\t"
            "lwc1       %[src6],    24(%[src])              \n\t"
            "lwc1       %[src7],    28(%[src])              \n\t"
            "mul.s      %[src0],    %[src0],    %[scale]    \n\t"
            "mul.s      %[src1],    %[src1],    %[scale]    \n\t"
            "mul.s      %[src2],    %[src2],    %[scale]    \n\t"
            "mul.s      %[src3],    %[src3],    %[scale]    \n\t"
            "mul.s      %[src4],    %[src4],    %[scale]    \n\t"
            "mul.s      %[src5],    %[src5],    %[scale]    \n\t"
            "mul.s      %[src6],    %[src6],    %[scale]    \n\t"
            "mul.s      %[src7],    %[src7],    %[scale]    \n\t"
            "cvt.w.s    %[src0],    %[src0]                 \n\t"
            "cvt.w.s    %[src1],    %[src1]                 \n\t"
            "cvt.w.s    %[src2],    %[src2]                 \n\t"
            "cvt.w.s    %[src3],    %[src3]                 \n\t"
            "cvt.w.s    %[src4],    %[src4]                 \n\t"
            "cvt.w.s    %[src5],    %[src5]                 \n\t"
            "cvt.w.s    %[src6],    %[src6]                 \n\t"
            "cvt.w.s    %[src7],    %[src7]                 \n\t"
            "mfc1       %[temp0],   %[src0]                 \n\t"
            "mfc1       %[temp1],   %[src1]                 \n\t"
            "mfc1       %[temp2],   %[src2]                 \n\t"
            "mfc1       %[temp3],   %[src3]                 \n\t"
            "mfc1       %[temp4],   %[src4]                 \n\t"
            "mfc1       %[temp5],   %[src5]                 \n\t"
            "mfc1       %[temp6],   %[src6]                 \n\t"
            "mfc1       %[temp7],   %[src7]                 \n\t"
            "sw         %[temp0],   0(%[dst])               \n\t"
            "sw         %[temp1],   4(%[dst])               \n\t"
            "sw         %[temp2],   8(%[dst])               \n\t"
            "sw         %[temp3],   12(%[dst])              \n\t"
            "sw         %[temp4],   16(%[dst])              \n\t"
            "sw         %[temp5],   20(%[dst])              \n\t"
            "sw         %[temp6],   24(%[dst])              \n\t"
            "sw         %[temp7],   28(%[dst])              \n\t"
            : [dst] "+r" (dst), [src] "+r" (src),
              [src0] "=&f" (src0), [src1] "=&f" (src1),
              [src2] "=&f" (src2), [src3] "=&f" (src3),
              [src4] "=&f" (src4), [src5] "=&f" (src5),
              [src6] "=&f" (src6), [src7] "=&f" (src7),
              [temp0] "=r" (temp0), [temp1] "=r" (temp1),
              [temp2] "=r" (temp2), [temp3] "=r" (temp3),
              [temp4] "=r" (temp4), [temp5] "=r" (temp5),
              [temp6] "=r" (temp6), [temp7] "=r" (temp7)
            : [scale] "f" (scale)
            : "memory"
        );
        src = src + 8;
        dst = dst + 8;
        len -= 8;
    } while (len > 0);
}
