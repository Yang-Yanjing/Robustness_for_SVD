static void set_special_band_scalefactors(AACEncContext *s, SingleChannelElement *sce)
{
    int w, g, start = 0;
    int minscaler_n = sce->sf_idx[0], minscaler_i = sce->sf_idx[0];
    int bands = 0;
    for (w = 0; w < sce->ics.num_windows; w += sce->ics.group_len[w]) {
        start = 0;
        for (g = 0;  g < sce->ics.num_swb; g++) {
            if (sce->band_type[w*16+g] == INTENSITY_BT || sce->band_type[w*16+g] == INTENSITY_BT2) {
                sce->sf_idx[w*16+g] = av_clip(ceilf(log2f(sce->is_ener[w*16+g])*2), -155, 100);
                minscaler_i = FFMIN(minscaler_i, sce->sf_idx[w*16+g]);
                bands++;
            } else if (sce->band_type[w*16+g] == NOISE_BT) {
                sce->sf_idx[w*16+g] = av_clip(4+log2f(sce->pns_ener[w*16+g])*2, -100, 155);
                minscaler_n = FFMIN(minscaler_n, sce->sf_idx[w*16+g]);
                bands++;
            }
            start += sce->ics.swb_sizes[g];
        }
    }
    if (!bands)
        return;
    
    for (w = 0; w < sce->ics.num_windows; w += sce->ics.group_len[w]) {
        for (g = 0;  g < sce->ics.num_swb; g++) {
            if (sce->band_type[w*16+g] == INTENSITY_BT || sce->band_type[w*16+g] == INTENSITY_BT2) {
                sce->sf_idx[w*16+g] = av_clip(sce->sf_idx[w*16+g], minscaler_i, minscaler_i + SCALE_MAX_DIFF);
            } else if (sce->band_type[w*16+g] == NOISE_BT) {
                sce->sf_idx[w*16+g] = av_clip(sce->sf_idx[w*16+g], minscaler_n, minscaler_n + SCALE_MAX_DIFF);
            }
        }
    }
}
