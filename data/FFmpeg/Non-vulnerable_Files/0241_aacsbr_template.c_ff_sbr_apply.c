void AAC_RENAME(ff_sbr_apply)(AACContext *ac, SpectralBandReplication *sbr, int id_aac,
                  INTFLOAT* L, INTFLOAT* R)
{
    int downsampled = ac->oc[1].m4ac.ext_sample_rate < sbr->sample_rate;
    int ch;
    int nch = (id_aac == TYPE_CPE) ? 2 : 1;
    int err;
    if (id_aac != sbr->id_aac) {
        av_log(ac->avctx, AV_LOG_ERROR,
            "element type mismatch %d != %d\n", id_aac, sbr->id_aac);
        sbr_turnoff(sbr);
    }
    if (!sbr->kx_and_m_pushed) {
        sbr->kx[0] = sbr->kx[1];
        sbr->m[0] = sbr->m[1];
    } else {
        sbr->kx_and_m_pushed = 0;
    }
    if (sbr->start) {
        sbr_dequant(sbr, id_aac);
    }
    for (ch = 0; ch < nch; ch++) {
        
        sbr_qmf_analysis(ac->fdsp, &sbr->mdct_ana, &sbr->dsp, ch ? R : L, sbr->data[ch].analysis_filterbank_samples,
                         (INTFLOAT*)sbr->qmf_filter_scratch,
                         sbr->data[ch].W, sbr->data[ch].Ypos);
        sbr->c.sbr_lf_gen(ac, sbr, sbr->X_low,
                          (const INTFLOAT (*)[32][32][2]) sbr->data[ch].W,
                          sbr->data[ch].Ypos);
        sbr->data[ch].Ypos ^= 1;
        if (sbr->start) {
            sbr->c.sbr_hf_inverse_filter(&sbr->dsp, sbr->alpha0, sbr->alpha1,
                                         (const INTFLOAT (*)[40][2]) sbr->X_low, sbr->k[0]);
            sbr_chirp(sbr, &sbr->data[ch]);
            av_assert0(sbr->data[ch].bs_num_env > 0);
            sbr_hf_gen(ac, sbr, sbr->X_high,
                       (const INTFLOAT (*)[40][2]) sbr->X_low,
                       (const INTFLOAT (*)[2]) sbr->alpha0,
                       (const INTFLOAT (*)[2]) sbr->alpha1,
                       sbr->data[ch].bw_array, sbr->data[ch].t_env,
                       sbr->data[ch].bs_num_env);
            
            err = sbr_mapping(ac, sbr, &sbr->data[ch], sbr->data[ch].e_a);
            if (!err) {
                sbr_env_estimate(sbr->e_curr, sbr->X_high, sbr, &sbr->data[ch]);
                sbr_gain_calc(ac, sbr, &sbr->data[ch], sbr->data[ch].e_a);
                sbr->c.sbr_hf_assemble(sbr->data[ch].Y[sbr->data[ch].Ypos],
                                (const INTFLOAT (*)[40][2]) sbr->X_high,
                                sbr, &sbr->data[ch],
                                sbr->data[ch].e_a);
            }
        }
        
        sbr->c.sbr_x_gen(sbr, sbr->X[ch],
                  (const INTFLOAT (*)[64][2]) sbr->data[ch].Y[1-sbr->data[ch].Ypos],
                  (const INTFLOAT (*)[64][2]) sbr->data[ch].Y[  sbr->data[ch].Ypos],
                  (const INTFLOAT (*)[40][2]) sbr->X_low, ch);
    }
    if (ac->oc[1].m4ac.ps == 1) {
        if (sbr->ps.start) {
            AAC_RENAME(ff_ps_apply)(ac->avctx, &sbr->ps, sbr->X[0], sbr->X[1], sbr->kx[1] + sbr->m[1]);
        } else {
            memcpy(sbr->X[1], sbr->X[0], sizeof(sbr->X[0]));
        }
        nch = 2;
    }
    sbr_qmf_synthesis(&sbr->mdct, &sbr->dsp, ac->fdsp,
                      L, sbr->X[0], sbr->qmf_filter_scratch,
                      sbr->data[0].synthesis_filterbank_samples,
                      &sbr->data[0].synthesis_filterbank_samples_offset,
                      downsampled);
    if (nch == 2)
        sbr_qmf_synthesis(&sbr->mdct, &sbr->dsp, ac->fdsp,
                          R, sbr->X[1], sbr->qmf_filter_scratch,
                          sbr->data[1].synthesis_filterbank_samples,
                          &sbr->data[1].synthesis_filterbank_samples_offset,
                          downsampled);
}
