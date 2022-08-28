static void ac3_update_bap_counts_mips(uint16_t mant_cnt[16], uint8_t *bap,
                                       int len)
{
    void *temp0, *temp2, *temp4, *temp5, *temp6, *temp7;
    int temp1, temp3;
    __asm__ volatile (
        "andi   %[temp3],   %[len],         3               \n\t"
        PTR_ADDU "%[temp2], %[bap],         %[len]          \n\t"
        PTR_ADDU "%[temp4], %[bap],         %[temp3]        \n\t"
        "beq    %[temp2],   %[temp4],       4f              \n\t"
        "1:                                                 \n\t"
        "lbu    %[temp0],   -1(%[temp2])                    \n\t"
        "lbu    %[temp5],   -2(%[temp2])                    \n\t"
        "lbu    %[temp6],   -3(%[temp2])                    \n\t"
        "sll    %[temp0],   %[temp0],       1               \n\t"
        PTR_ADDU "%[temp0], %[mant_cnt],    %[temp0]        \n\t"
        "sll    %[temp5],   %[temp5],       1               \n\t"
        PTR_ADDU "%[temp5], %[mant_cnt],    %[temp5]        \n\t"
        "lhu    %[temp1],   0(%[temp0])                     \n\t"
        "sll    %[temp6],   %[temp6],       1               \n\t"
        PTR_ADDU "%[temp6], %[mant_cnt],    %[temp6]        \n\t"
        "addiu  %[temp1],   %[temp1],       1               \n\t"
        "sh     %[temp1],   0(%[temp0])                     \n\t"
        "lhu    %[temp1],   0(%[temp5])                     \n\t"
        "lbu    %[temp7],   -4(%[temp2])                    \n\t"
        PTR_ADDIU "%[temp2],%[temp2],       -4              \n\t"
        "addiu  %[temp1],   %[temp1],       1               \n\t"
        "sh     %[temp1],   0(%[temp5])                     \n\t"
        "lhu    %[temp1],   0(%[temp6])                     \n\t"
        "sll    %[temp7],   %[temp7],       1               \n\t"
        PTR_ADDU "%[temp7], %[mant_cnt],    %[temp7]        \n\t"
        "addiu  %[temp1],   %[temp1],1                      \n\t"
        "sh     %[temp1],   0(%[temp6])                     \n\t"
        "lhu    %[temp1],   0(%[temp7])                     \n\t"
        "addiu  %[temp1],   %[temp1],       1               \n\t"
        "sh     %[temp1],   0(%[temp7])                     \n\t"
        "bne    %[temp2],   %[temp4],       1b              \n\t"
        "4:                                                 \n\t"
        "beqz   %[temp3],   2f                              \n\t"
        "3:                                                 \n\t"
        "addiu  %[temp3],   %[temp3],       -1              \n\t"
        "lbu    %[temp0],   -1(%[temp2])                    \n\t"
        PTR_ADDIU "%[temp2],%[temp2],       -1              \n\t"
        "sll    %[temp0],   %[temp0],       1               \n\t"
        PTR_ADDU "%[temp0], %[mant_cnt],    %[temp0]        \n\t"
        "lhu    %[temp1],   0(%[temp0])                     \n\t"
        "addiu  %[temp1],   %[temp1],       1               \n\t"
        "sh     %[temp1],   0(%[temp0])                     \n\t"
        "bgtz   %[temp3],   3b                              \n\t"
        "2:                                                 \n\t"
        : [temp0] "=&r" (temp0), [temp1] "=&r" (temp1),
          [temp2] "=&r" (temp2), [temp3] "=&r" (temp3),
          [temp4] "=&r" (temp4), [temp5] "=&r" (temp5),
          [temp6] "=&r" (temp6), [temp7] "=&r" (temp7)
        : [len] "r" (len), [bap] "r" (bap),
          [mant_cnt] "r" (mant_cnt)
        : "memory"
    );
}
