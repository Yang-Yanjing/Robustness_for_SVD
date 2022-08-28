static void ac3_bit_alloc_calc_bap_mips(int16_t *mask, int16_t *psd,
                                        int start, int end,
                                        int snr_offset, int floor,
                                        const uint8_t *bap_tab, uint8_t *bap)
{
    int band, band_end, cond;
    int m, address1, address2;
    int16_t *psd1, *psd_end;
    uint8_t *bap1;
    if (snr_offset == -960) {
        memset(bap, 0, AC3_MAX_COEFS);
        return;
    }
    psd1 = &psd[start];
    bap1 = &bap[start];
    band = ff_ac3_bin_to_band_tab[start];
    do {
        m = (FFMAX(mask[band] - snr_offset - floor, 0) & 0x1FE0) + floor;
        band_end = ff_ac3_band_start_tab[++band];
        band_end = FFMIN(band_end, end);
        psd_end = psd + band_end - 1;
        __asm__ volatile (
            "slt        %[cond],        %[psd1],        %[psd_end]  \n\t"
            "beqz       %[cond],        1f                          \n\t"
            "2:                                                     \n\t"
            "lh         %[address1],    0(%[psd1])                  \n\t"
            "lh         %[address2],    2(%[psd1])                  \n\t"
            PTR_ADDIU " %[psd1],        %[psd1],        4           \n\t"
            "subu       %[address1],    %[address1],    %[m]        \n\t"
            "sra        %[address1],    %[address1],    5           \n\t"
            "addiu      %[address1],    %[address1],    -32         \n\t"
            "shll_s.w   %[address1],    %[address1],    26          \n\t"
            "subu       %[address2],    %[address2],    %[m]        \n\t"
            "sra        %[address2],    %[address2],    5           \n\t"
            "sra        %[address1],    %[address1],    26          \n\t"
            "addiu      %[address1],    %[address1],    32          \n\t"
            "lbux       %[address1],    %[address1](%[bap_tab])     \n\t"
            "addiu      %[address2],    %[address2],    -32         \n\t"
            "shll_s.w   %[address2],    %[address2],    26          \n\t"
            "sb         %[address1],    0(%[bap1])                  \n\t"
            "slt        %[cond],        %[psd1],        %[psd_end]  \n\t"
            "sra        %[address2],    %[address2],    26          \n\t"
            "addiu      %[address2],    %[address2],    32          \n\t"
            "lbux       %[address2],    %[address2](%[bap_tab])     \n\t"
            "sb         %[address2],    1(%[bap1])                  \n\t"
            PTR_ADDIU " %[bap1],        %[bap1],        2           \n\t"
            "bnez       %[cond],        2b                          \n\t"
            PTR_ADDIU " %[psd_end],     %[psd_end],     2           \n\t"
            "slt        %[cond],        %[psd1],        %[psd_end]  \n\t"
            "beqz       %[cond],        3f                          \n\t"
            "1:                                                     \n\t"
            "lh         %[address1],    0(%[psd1])                  \n\t"
            PTR_ADDIU " %[psd1],        %[psd1],        2           \n\t"
            "subu       %[address1],    %[address1],    %[m]        \n\t"
            "sra        %[address1],    %[address1],    5           \n\t"
            "addiu      %[address1],    %[address1],    -32         \n\t"
            "shll_s.w   %[address1],    %[address1],    26          \n\t"
            "sra        %[address1],    %[address1],    26          \n\t"
            "addiu      %[address1],    %[address1],    32          \n\t"
            "lbux       %[address1],    %[address1](%[bap_tab])     \n\t"
            "sb         %[address1],    0(%[bap1])                  \n\t"
            PTR_ADDIU " %[bap1],        %[bap1],        1           \n\t"
            "3:                                                     \n\t"
            : [address1]"=&r"(address1), [address2]"=&r"(address2),
              [cond]"=&r"(cond), [bap1]"+r"(bap1),
              [psd1]"+r"(psd1), [psd_end]"+r"(psd_end)
            : [m]"r"(m), [bap_tab]"r"(bap_tab)
            : "memory"
        );
    } while (end > band_end);
}
