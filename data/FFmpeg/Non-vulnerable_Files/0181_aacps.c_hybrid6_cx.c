static void hybrid6_cx(PSDSPContext *dsp, INTFLOAT (*in)[2], INTFLOAT (*out)[32][2],
                       TABLE_CONST INTFLOAT (*filter)[8][2], int len)
{
    int i;
    int N = 8;
    LOCAL_ALIGNED_16(INTFLOAT, temp, [8], [2]);
    for (i = 0; i < len; i++, in++) {
        dsp->hybrid_analysis(temp, in, (const INTFLOAT (*)[8][2]) filter, 1, N);
        out[0][i][0] = temp[6][0];
        out[0][i][1] = temp[6][1];
        out[1][i][0] = temp[7][0];
        out[1][i][1] = temp[7][1];
        out[2][i][0] = temp[0][0];
        out[2][i][1] = temp[0][1];
        out[3][i][0] = temp[1][0];
        out[3][i][1] = temp[1][1];
        out[4][i][0] = temp[2][0] + temp[5][0];
        out[4][i][1] = temp[2][1] + temp[5][1];
        out[5][i][0] = temp[3][0] + temp[4][0];
        out[5][i][1] = temp[3][1] + temp[4][1];
    }
}
