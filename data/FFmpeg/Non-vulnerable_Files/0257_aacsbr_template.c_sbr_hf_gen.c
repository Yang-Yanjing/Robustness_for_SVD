
static int sbr_hf_gen(AACContext *ac, SpectralBandReplication *sbr,
                      INTFLOAT X_high[64][40][2], const INTFLOAT X_low[32][40][2],
                      const INTFLOAT (*alpha0)[2], const INTFLOAT (*alpha1)[2],
                      const INTFLOAT bw_array[5], const uint8_t *t_env,
                      int bs_num_env)
{
    int j, x;
    int g = 0;
    int k = sbr->kx[1];
    for (j = 0; j < sbr->num_patches; j++) {
        for (x = 0; x < sbr->patch_num_subbands[j]; x++, k++) {
            const int p = sbr->patch_start_subband[j] + x;
            while (g <= sbr->n_q && k >= sbr->f_tablenoise[g])
                g++;
            g--;
            if (g < 0) {
                av_log(ac->avctx, AV_LOG_ERROR,
                       "ERROR : no subband found for frequency %d\n", k);
                return -1;
            }
            sbr->dsp.hf_gen(X_high[k] + ENVELOPE_ADJUSTMENT_OFFSET,
                            X_low[p]  + ENVELOPE_ADJUSTMENT_OFFSET,
                            alpha0[p], alpha1[p], bw_array[g],
                            2 * t_env[0], 2 * t_env[bs_num_env]);
        }
    }
    if (k < sbr->m[1] + sbr->kx[1])
        memset(X_high + k, 0, (sbr->m[1] + sbr->kx[1] - k) * sizeof(*X_high));
    return 0;
}
