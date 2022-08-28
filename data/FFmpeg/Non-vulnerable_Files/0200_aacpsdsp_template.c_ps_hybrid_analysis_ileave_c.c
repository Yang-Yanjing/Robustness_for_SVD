static void ps_hybrid_analysis_ileave_c(INTFLOAT (*out)[32][2], INTFLOAT L[2][38][64],
                                      int i, int len)
{
    int j;
    for (; i < 64; i++) {
        for (j = 0; j < len; j++) {
            out[i][j][0] = L[0][j][i];
            out[i][j][1] = L[1][j][i];
        }
    }
}
