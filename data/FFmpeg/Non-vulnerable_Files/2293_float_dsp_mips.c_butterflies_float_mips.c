static void butterflies_float_mips(float *av_restrict v1, float *av_restrict v2,
                                int len)
{
    float temp0, temp1, temp2, temp3, temp4;
    float temp5, temp6, temp7, temp8, temp9;
    float temp10, temp11, temp12, temp13, temp14, temp15;
    int pom;
    pom = (len >> 2)-1;
    
    __asm__ volatile (
        "lwc1     %[temp0],    0(%[v1])                 \n\t"
        "lwc1     %[temp1],    4(%[v1])                 \n\t"
        "lwc1     %[temp2],    8(%[v1])                 \n\t"
        "lwc1     %[temp3],    12(%[v1])                \n\t"
        "lwc1     %[temp4],    0(%[v2])                 \n\t"
        "lwc1     %[temp5],    4(%[v2])                 \n\t"
        "lwc1     %[temp6],    8(%[v2])                 \n\t"
        "lwc1     %[temp7],    12(%[v2])                \n\t"
        "beq      %[pom],      $zero,       2f          \n\t"
    "1:                                                 \n\t"
        "sub.s    %[temp8],    %[temp0],    %[temp4]    \n\t"
        "add.s    %[temp9],    %[temp0],    %[temp4]    \n\t"
        "sub.s    %[temp10],   %[temp1],    %[temp5]    \n\t"
        "add.s    %[temp11],   %[temp1],    %[temp5]    \n\t"
        "sub.s    %[temp12],   %[temp2],    %[temp6]    \n\t"
        "add.s    %[temp13],   %[temp2],    %[temp6]    \n\t"
        "sub.s    %[temp14],   %[temp3],    %[temp7]    \n\t"
        "add.s    %[temp15],   %[temp3],    %[temp7]    \n\t"
        PTR_ADDIU "%[v1],      %[v1],       16          \n\t"
        PTR_ADDIU "%[v2],      %[v2],       16          \n\t"
        "addiu    %[pom],      %[pom],      -1          \n\t"
        "lwc1     %[temp0],    0(%[v1])                 \n\t"
        "lwc1     %[temp1],    4(%[v1])                 \n\t"
        "lwc1     %[temp2],    8(%[v1])                 \n\t"
        "lwc1     %[temp3],    12(%[v1])                \n\t"
        "lwc1     %[temp4],    0(%[v2])                 \n\t"
        "lwc1     %[temp5],    4(%[v2])                 \n\t"
        "lwc1     %[temp6],    8(%[v2])                 \n\t"
        "lwc1     %[temp7],    12(%[v2])                \n\t"
        "swc1     %[temp9],    -16(%[v1])               \n\t"
        "swc1     %[temp8],    -16(%[v2])               \n\t"
        "swc1     %[temp11],   -12(%[v1])               \n\t"
        "swc1     %[temp10],   -12(%[v2])               \n\t"
        "swc1     %[temp13],   -8(%[v1])                \n\t"
        "swc1     %[temp12],   -8(%[v2])                \n\t"
        "swc1     %[temp15],   -4(%[v1])                \n\t"
        "swc1     %[temp14],   -4(%[v2])                \n\t"
        "bgtz     %[pom],      1b                       \n\t"
    "2:                                                 \n\t"
        "sub.s    %[temp8],    %[temp0],    %[temp4]    \n\t"
        "add.s    %[temp9],    %[temp0],    %[temp4]    \n\t"
        "sub.s    %[temp10],   %[temp1],    %[temp5]    \n\t"
        "add.s    %[temp11],   %[temp1],    %[temp5]    \n\t"
        "sub.s    %[temp12],   %[temp2],    %[temp6]    \n\t"
        "add.s    %[temp13],   %[temp2],    %[temp6]    \n\t"
        "sub.s    %[temp14],   %[temp3],    %[temp7]    \n\t"
        "add.s    %[temp15],   %[temp3],    %[temp7]    \n\t"
        "swc1     %[temp9],    0(%[v1])                 \n\t"
        "swc1     %[temp8],    0(%[v2])                 \n\t"
        "swc1     %[temp11],   4(%[v1])                 \n\t"
        "swc1     %[temp10],   4(%[v2])                 \n\t"
        "swc1     %[temp13],   8(%[v1])                 \n\t"
        "swc1     %[temp12],   8(%[v2])                 \n\t"
        "swc1     %[temp15],   12(%[v1])                \n\t"
        "swc1     %[temp14],   12(%[v2])                \n\t"
        : [v1]"+r"(v1), [v2]"+r"(v2), [pom]"+r"(pom), [temp0] "=&f" (temp0),
          [temp1]"=&f"(temp1), [temp2]"=&f"(temp2), [temp3]"=&f"(temp3),
          [temp4]"=&f"(temp4), [temp5]"=&f"(temp5), [temp6]"=&f"(temp6),
          [temp7]"=&f"(temp7), [temp8]"=&f"(temp8), [temp9]"=&f"(temp9),
          [temp10]"=&f"(temp10), [temp11]"=&f"(temp11), [temp12]"=&f"(temp12),
          [temp13]"=&f"(temp13), [temp14]"=&f"(temp14), [temp15]"=&f"(temp15)
        :
        : "memory"
    );
}
