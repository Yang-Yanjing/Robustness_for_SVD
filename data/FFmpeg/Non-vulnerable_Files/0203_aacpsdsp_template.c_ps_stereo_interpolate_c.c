static void ps_stereo_interpolate_c(INTFLOAT (*l)[2], INTFLOAT (*r)[2],
                                    INTFLOAT h[2][4], INTFLOAT h_step[2][4],
                                    int len)
{
    INTFLOAT h0 = h[0][0];
    INTFLOAT h1 = h[0][1];
    INTFLOAT h2 = h[0][2];
    INTFLOAT h3 = h[0][3];
    INTFLOAT hs0 = h_step[0][0];
    INTFLOAT hs1 = h_step[0][1];
    INTFLOAT hs2 = h_step[0][2];
    INTFLOAT hs3 = h_step[0][3];
    int n;
    for (n = 0; n < len; n++) {
        
        INTFLOAT l_re = l[n][0];
        INTFLOAT l_im = l[n][1];
        INTFLOAT r_re = r[n][0];
        INTFLOAT r_im = r[n][1];
        h0 += hs0;
        h1 += hs1;
        h2 += hs2;
        h3 += hs3;
        l[n][0] = AAC_MADD30(h0,  l_re,  h2, r_re);
        l[n][1] = AAC_MADD30(h0,  l_im,  h2,  r_im);
        r[n][0] = AAC_MADD30(h1,  l_re,  h3,  r_re);
        r[n][1] = AAC_MADD30(h1,  l_im,  h3,  r_im);
    }
}
