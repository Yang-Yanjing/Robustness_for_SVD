
static void sbr_env_estimate(AAC_FLOAT (*e_curr)[48], INTFLOAT X_high[64][40][2],
                             SpectralBandReplication *sbr, SBRData *ch_data)
{
    int e, m;
    int kx1 = sbr->kx[1];
    if (sbr->bs_interpol_freq) {
        for (e = 0; e < ch_data->bs_num_env; e++) {
#if USE_FIXED
            const SoftFloat recip_env_size = av_int2sf(0x20000000 / (ch_data->t_env[e + 1] - ch_data->t_env[e]), 30);
#else
            const float recip_env_size = 0.5f / (ch_data->t_env[e + 1] - ch_data->t_env[e]);
#endif 
            int ilb = ch_data->t_env[e]     * 2 + ENVELOPE_ADJUSTMENT_OFFSET;
            int iub = ch_data->t_env[e + 1] * 2 + ENVELOPE_ADJUSTMENT_OFFSET;
            for (m = 0; m < sbr->m[1]; m++) {
                AAC_FLOAT sum = sbr->dsp.sum_square(X_high[m+kx1] + ilb, iub - ilb);
#if USE_FIXED
                e_curr[e][m] = av_mul_sf(sum, recip_env_size);
#else
                e_curr[e][m] = sum * recip_env_size;
#endif 
            }
        }
    } else {
        int k, p;
        for (e = 0; e < ch_data->bs_num_env; e++) {
            const int env_size = 2 * (ch_data->t_env[e + 1] - ch_data->t_env[e]);
            int ilb = ch_data->t_env[e]     * 2 + ENVELOPE_ADJUSTMENT_OFFSET;
            int iub = ch_data->t_env[e + 1] * 2 + ENVELOPE_ADJUSTMENT_OFFSET;
            const uint16_t *table = ch_data->bs_freq_res[e + 1] ? sbr->f_tablehigh : sbr->f_tablelow;
            for (p = 0; p < sbr->n[ch_data->bs_freq_res[e + 1]]; p++) {
#if USE_FIXED
                SoftFloat sum = { 0, 0 };
                const SoftFloat den = av_int2sf(0x20000000 / (env_size * (table[p + 1] - table[p])), 29);
                for (k = table[p]; k < table[p + 1]; k++) {
                    sum = av_add_sf(sum, sbr->dsp.sum_square(X_high[k] + ilb, iub - ilb));
                }
                sum = av_mul_sf(sum, den);
#else
                float sum = 0.0f;
                const int den = env_size * (table[p + 1] - table[p]);
                for (k = table[p]; k < table[p + 1]; k++) {
                    sum += sbr->dsp.sum_square(X_high[k] + ilb, iub - ilb);
                }
                sum /= den;
#endif 
                for (k = table[p]; k < table[p + 1]; k++) {
                    e_curr[e][k - kx1] = sum;
                }
            }
        }
    }
}
