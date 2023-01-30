static void encode_spectral_coeffs(AACEncContext *s, SingleChannelElement *sce)
{
    int start, i, w, w2;
    for (w = 0; w < sce->ics.num_windows; w += sce->ics.group_len[w]) {
        start = 0;
        for (i = 0; i < sce->ics.max_sfb; i++) {
            if (sce->zeroes[w*16 + i]) {
                start += sce->ics.swb_sizes[i];
                continue;
            }
            for (w2 = w; w2 < w + sce->ics.group_len[w]; w2++) {
                s->coder->quantize_and_encode_band(s, &s->pb,
                                                   &sce->coeffs[start + w2*128],
                                                   NULL, sce->ics.swb_sizes[i],
                                                   sce->sf_idx[w*16 + i],
                                                   sce->band_type[w*16 + i],
                                                   s->lambda,
                                                   sce->ics.window_clipping[w]);
            }
            start += sce->ics.swb_sizes[i];
        }
    }
}
