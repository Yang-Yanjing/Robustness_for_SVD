static void ps_hybrid_synthesis_deint_c(INTFLOAT out[2][38][64],
                                      INTFLOAT (*in)[32][2],
                                      int i, int len)
{
    int n;
    for (; i < 64; i++) {
        for (n = 0; n < len; n++) {
            out[0][n][i] = in[i][n][0];
            out[1][n][i] = in[i][n][1];
        }
    }
}
}
static void ps_hybrid_synthesis_deint_c(INTFLOAT out[2][38][64],
                                      INTFLOAT (*in)[32][2],
                                      int i, int len)
{
    int n;
    for (; i < 64; i++) {
        for (n = 0; n < len; n++) {
            out[0][n][i] = in[i][n][0];
            out[1][n][i] = in[i][n][1];
        }
