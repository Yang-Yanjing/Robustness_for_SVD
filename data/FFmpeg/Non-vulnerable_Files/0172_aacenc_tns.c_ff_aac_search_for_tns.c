void ff_aac_search_for_tns(AACEncContext *s, SingleChannelElement *sce)
{
    TemporalNoiseShaping *tns = &sce->tns;
    int w, w2, g, count = 0;
    const int mmm = FFMIN(sce->ics.tns_max_bands, sce->ics.max_sfb);
    const int is8 = sce->ics.window_sequence[0] == EIGHT_SHORT_SEQUENCE;
    const int order = is8 ? 7 : s->profile == FF_PROFILE_AAC_LOW ? 12 : TNS_MAX_ORDER;
    int sfb_start = av_clip(tns_min_sfb[is8][s->samplerate_index], 0, mmm);
    int sfb_end   = av_clip(sce->ics.num_swb, 0, mmm);
    for (w = 0; w < sce->ics.num_windows; w++) {
        float e_ratio = 0.0f, threshold = 0.0f, spread = 0.0f, en[2] = {0.0, 0.0f};
        double gain = 0.0f, coefs[MAX_LPC_ORDER] = {0};
        int coef_start = w*sce->ics.num_swb + sce->ics.swb_offset[sfb_start];
        int coef_len = sce->ics.swb_offset[sfb_end] - sce->ics.swb_offset[sfb_start];
        for (g = 0;  g < sce->ics.num_swb; g++) {
            if (w*16+g < sfb_start || w*16+g > sfb_end)
                continue;
            for (w2 = 0; w2 < sce->ics.group_len[w]; w2++) {
                FFPsyBand *band = &s->psy.ch[s->cur_channel].psy_bands[(w+w2)*16+g];
                if ((w+w2)*16+g > sfb_start + ((sfb_end - sfb_start)/2))
                    en[1] += band->energy;
                else
                    en[0] += band->energy;
                threshold += band->threshold;
                spread += band->spread;
            }
        }
        if (coef_len <= 0 || (sfb_end - sfb_start) <= 0)
            continue;
        else
            e_ratio = en[0]/en[1];
        
        gain = ff_lpc_calc_ref_coefs_f(&s->lpc, &sce->coeffs[coef_start],
                                       coef_len, order, coefs);
        if (gain > TNS_GAIN_THRESHOLD_LOW && gain < TNS_GAIN_THRESHOLD_HIGH &&
            (en[0]+en[1]) > TNS_GAIN_THRESHOLD_LOW*threshold &&
            spread < TNS_SPREAD_THRESHOLD && order) {
            if (is8 || order < 2 || (e_ratio > TNS_E_RATIO_LOW && e_ratio < TNS_E_RATIO_HIGH)) {
                tns->n_filt[w] = 1;
                for (g = 0; g < tns->n_filt[w]; g++) {
                    tns->length[w][g] = sfb_end - sfb_start;
                    tns->direction[w][g] = en[0] < en[1];
                    tns->order[w][g] = order;
                    quantize_coefs(coefs, tns->coef_idx[w][g], tns->coef[w][g],
                                   order);
                }
            } else {  
                tns->n_filt[w] = 2;
                for (g = 0; g < tns->n_filt[w]; g++) {
                    tns->direction[w][g] = en[g] < en[!g];
                    tns->order[w][g] = !g ? order/2 : order - tns->order[w][g-1];
                    tns->length[w][g] = !g ? (sfb_end - sfb_start)/2 : \
                                    (sfb_end - sfb_start) - tns->length[w][g-1];
                    quantize_coefs(&coefs[!g ? 0 : order - tns->order[w][g-1]],
                                   tns->coef_idx[w][g], tns->coef[w][g],
                                   tns->order[w][g]);
                }
            }
            count++;
        }
    }
    sce->tns.present = !!count;
}
