static void hybrid4_8_12_cx(PSDSPContext *dsp,
                            INTFLOAT (*in)[2], INTFLOAT (*out)[32][2],
                            TABLE_CONST INTFLOAT (*filter)[8][2], int N, int len)
{
    int i;
    for (i = 0; i < len; i++, in++) {
        dsp->hybrid_analysis(out[0] + i, in, (const INTFLOAT (*)[8][2]) filter, 32, N);
    }
}
