static void row_idct_c(int16_t *workspace, int16_t *output_adr, ptrdiff_t output_stride, int cnt)
{
    int_simd16_t tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;
    int_simd16_t tmp10, tmp11, tmp12, tmp13;
    int_simd16_t z5, z10, z11, z12, z13;
    int16_t *outptr;
    int16_t *wsptr;
    cnt *= 4;
    wsptr = workspace;
    outptr = output_adr;
    for (; cnt > 0; cnt--) {
        
        
        tmp10 = wsptr[2] +  wsptr[3];
        tmp11 = wsptr[2] -  wsptr[3];
        tmp13 = wsptr[0] +  wsptr[1];
        tmp12 = (MULTIPLY16H(wsptr[0] - wsptr[1], FIX_1_414213562_A) << 2) - tmp13;
        tmp0 = tmp10 + tmp13; 
        tmp3 = tmp10 - tmp13; 
        tmp1 = tmp11 + tmp12;
        tmp2 = tmp11 - tmp12;
        
        
        
        
        
        
        z13 = wsptr[4] + wsptr[5];
        z10 = wsptr[4] - wsptr[5];
        z11 = wsptr[6] + wsptr[7];
        z12 = wsptr[6] - wsptr[7];
        tmp7 = z11 + z13;
        tmp11 = MULTIPLY16H(z11 - z13, FIX_1_414213562);
        z5 =    MULTIPLY16H(z10 + z12, FIX_1_847759065);
        tmp10 = MULTIPLY16H(z12,       FIX_1_082392200) - z5;
        tmp12 = MULTIPLY16H(z10,       FIX_2_613125930) + z5; 
        tmp6 = (tmp12 << 3) - tmp7;
        tmp5 = (tmp11 << 3) - tmp6;
        tmp4 = (tmp10 << 3) + tmp5;
        
        outptr[0 * output_stride] += DESCALE(tmp0 + tmp7, 3);
        outptr[1 * output_stride] += DESCALE(tmp1 + tmp6, 3);
        outptr[2 * output_stride] += DESCALE(tmp2 + tmp5, 3);
        outptr[3 * output_stride] += DESCALE(tmp3 - tmp4, 3);
        outptr[4 * output_stride] += DESCALE(tmp3 + tmp4, 3);
        outptr[5 * output_stride] += DESCALE(tmp2 - tmp5, 3);
        outptr[6 * output_stride] += DESCALE(tmp1 - tmp6, 3); 
        outptr[7 * output_stride] += DESCALE(tmp0 - tmp7, 3); 
        outptr++;
        wsptr += DCTSIZE;       
    }
}
