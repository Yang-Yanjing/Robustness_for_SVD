static void vector_fmul_scalar_mips(float *dst, const float *src, float mul,
                                 int len)
{
    float temp0, temp1, temp2, temp3;
    float *local_src = (float*)src;
    float *end = local_src + len;
    
    __asm__ volatile(
        ".set    push                             \n\t"
        ".set    noreorder                        \n\t"
    "1:                                           \n\t"
        "lwc1    %[temp0],   0(%[src])            \n\t"
        "lwc1    %[temp1],   4(%[src])            \n\t"
        "lwc1    %[temp2],   8(%[src])            \n\t"
        "lwc1    %[temp3],   12(%[src])           \n\t"
        PTR_ADDIU "%[dst],   %[dst],     16       \n\t"
        "mul.s   %[temp0],   %[temp0],   %[mul]   \n\t"
        "mul.s   %[temp1],   %[temp1],   %[mul]   \n\t"
        "mul.s   %[temp2],   %[temp2],   %[mul]   \n\t"
        "mul.s   %[temp3],   %[temp3],   %[mul]   \n\t"
        PTR_ADDIU "%[src],   %[src],     16       \n\t"
        "swc1    %[temp0],   -16(%[dst])          \n\t"
        "swc1    %[temp1],   -12(%[dst])          \n\t"
        "swc1    %[temp2],   -8(%[dst])           \n\t"
        "bne     %[src],     %[end],     1b       \n\t"
        " swc1   %[temp3],   -4(%[dst])           \n\t"
        ".set    pop                              \n\t"
        : [temp0]"=&f"(temp0), [temp1]"=&f"(temp1),
          [temp2]"=&f"(temp2), [temp3]"=&f"(temp3),
          [dst]"+r"(dst), [src]"+r"(local_src)
        : [end]"r"(end), [mul]"f"(mul)
        : "memory"
    );
}
