static void vector_fmul_reverse_mips(float *dst, const float *src0, const float *src1, int len){
    int i;
    float temp0, temp1, temp2, temp3, temp4, temp5, temp6, temp7;
    src1 += len-1;
    for(i=0; i<(len>>2); i++)
    {
        
        __asm__ volatile(
            "lwc1      %[temp0],     0(%[src0])                 \n\t"
            "lwc1      %[temp1],     0(%[src1])                 \n\t"
            "lwc1      %[temp2],     4(%[src0])                 \n\t"
            "lwc1      %[temp3],     -4(%[src1])                \n\t"
            "lwc1      %[temp4],     8(%[src0])                 \n\t"
            "lwc1      %[temp5],     -8(%[src1])                \n\t"
            "lwc1      %[temp6],     12(%[src0])                \n\t"
            "lwc1      %[temp7],     -12(%[src1])               \n\t"
            "mul.s     %[temp0],     %[temp1],     %[temp0]     \n\t"
            "mul.s     %[temp2],     %[temp3],     %[temp2]     \n\t"
            "mul.s     %[temp4],     %[temp5],     %[temp4]     \n\t"
            "mul.s     %[temp6],     %[temp7],     %[temp6]     \n\t"
            PTR_ADDIU "%[src0],      %[src0],      16           \n\t"
            PTR_ADDIU "%[src1],      %[src1],      -16          \n\t"
            PTR_ADDIU "%[dst],       %[dst],       16           \n\t"
            "swc1      %[temp0],     -16(%[dst])                \n\t"
            "swc1      %[temp2],     -12(%[dst])                \n\t"
            "swc1      %[temp4],     -8(%[dst])                 \n\t"
            "swc1      %[temp6],     -4(%[dst])                 \n\t"
            : [dst]"+r"(dst), [src0]"+r"(src0), [src1]"+r"(src1),
              [temp0]"=&f"(temp0), [temp1]"=&f"(temp1),[temp2]"=&f"(temp2),
              [temp3]"=&f"(temp3), [temp4]"=&f"(temp4), [temp5]"=&f"(temp5),
              [temp6]"=&f"(temp6), [temp7]"=&f"(temp7)
            :
            : "memory"
        );
    }
}
