static void ps_stereo_interpolate_ipdopd_c(INTFLOAT (*l)[2], INTFLOAT (*r)[2],
                                           INTFLOAT h[2][4], INTFLOAT h_step[2][4],
                                           int len)
{
    INTFLOAT h00  = h[0][0],      h10  = h[1][0];
    INTFLOAT h01  = h[0][1],      h11  = h[1][1];
    INTFLOAT h02  = h[0][2],      h12  = h[1][2];
    INTFLOAT h03  = h[0][3],      h13  = h[1][3];
    INTFLOAT hs00 = h_step[0][0], hs10 = h_step[1][0];
    INTFLOAT hs01 = h_step[0][1], hs11 = h_step[1][1];
    INTFLOAT hs02 = h_step[0][2], hs12 = h_step[1][2];
    INTFLOAT hs03 = h_step[0][3], hs13 = h_step[1][3];
    int n;
    for (n = 0; n < len; n++) {
        
        INTFLOAT l_re = l[n][0];
        INTFLOAT l_im = l[n][1];
        INTFLOAT r_re = r[n][0];
        INTFLOAT r_im = r[n][1];
        h00 += hs00;
        h01 += hs01;
        h02 += hs02;
        h03 += hs03;
        h10 += hs10;
        h11 += hs11;
        h12 += hs12;
        h13 += hs13;
        l[n][0] = AAC_MSUB30_V8(h00, l_re, h02, r_re, h10, l_im, h12, r_im);
        l[n][1] = AAC_MADD30_V8(h00, l_im, h02, r_im, h10, l_re, h12, r_re);
        r[n][0] = AAC_MSUB30_V8(h01, l_re, h03, r_re, h11, l_im, h13, r_im);
        r[n][1] = AAC_MADD30_V8(h01, l_im, h03, r_im, h11, l_re, h13, r_re);
    }
}
