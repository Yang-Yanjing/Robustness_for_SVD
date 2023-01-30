
static int sbr_lf_gen(AACContext *ac, SpectralBandReplication *sbr,
                      INTFLOAT X_low[32][40][2], const INTFLOAT W[2][32][32][2],
                      int buf_idx)
{
    int i, k;
    const int t_HFGen = 8;
    const int i_f = 32;
    memset(X_low, 0, 32*sizeof(*X_low));
    for (k = 0; k < sbr->kx[1]; k++) {
        for (i = t_HFGen; i < i_f + t_HFGen; i++) {
            X_low[k][i][0] = W[buf_idx][i - t_HFGen][k][0];
            X_low[k][i][1] = W[buf_idx][i - t_HFGen][k][1];
        }
    }
    buf_idx = 1-buf_idx;
    for (k = 0; k < sbr->kx[0]; k++) {
        for (i = 0; i < t_HFGen; i++) {
            X_low[k][i][0] = W[buf_idx][i + i_f - t_HFGen][k][0];
            X_low[k][i][1] = W[buf_idx][i + i_f - t_HFGen][k][1];
        }
    }
    return 0;
}
