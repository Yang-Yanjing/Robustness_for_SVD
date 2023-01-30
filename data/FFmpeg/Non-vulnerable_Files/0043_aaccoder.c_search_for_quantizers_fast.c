static void search_for_quantizers_fast(AVCodecContext *avctx, AACEncContext *s,
                                       SingleChannelElement *sce,
                                       const float lambda)
{
    int i, w, w2, g;
    int minq = 255;
    memset(sce->sf_idx, 0, sizeof(sce->sf_idx));
    for (w = 0; w < sce->ics.num_windows; w += sce->ics.group_len[w]) {
        for (g = 0; g < sce->ics.num_swb; g++) {
            for (w2 = 0; w2 < sce->ics.group_len[w]; w2++) {
                FFPsyBand *band = &s->psy.ch[s->cur_channel].psy_bands[(w+w2)*16+g];
                if (band->energy <= band->threshold) {
                    sce->sf_idx[(w+w2)*16+g] = 218;
                    sce->zeroes[(w+w2)*16+g] = 1;
                } else {
                    sce->sf_idx[(w+w2)*16+g] = av_clip(SCALE_ONE_POS - SCALE_DIV_512 + log2f(band->threshold), 80, 218);
                    sce->zeroes[(w+w2)*16+g] = 0;
                }
                minq = FFMIN(minq, sce->sf_idx[(w+w2)*16+g]);
            }
        }
    }
    for (i = 0; i < 128; i++) {
        sce->sf_idx[i] = 140;
        
    }
    
    for (w = 0; w < sce->ics.num_windows; w += sce->ics.group_len[w])
        for (g = 0;  g < sce->ics.num_swb; g++)
            for (w2 = 1; w2 < sce->ics.group_len[w]; w2++)
                sce->sf_idx[(w+w2)*16+g] = sce->sf_idx[w*16+g];
}
