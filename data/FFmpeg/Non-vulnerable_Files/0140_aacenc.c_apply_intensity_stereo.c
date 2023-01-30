static void apply_intensity_stereo(ChannelElement *cpe)
{
    int w, w2, g, i;
    IndividualChannelStream *ics = &cpe->ch[0].ics;
    if (!cpe->common_window)
        return;
    for (w = 0; w < ics->num_windows; w += ics->group_len[w]) {
        for (w2 =  0; w2 < ics->group_len[w]; w2++) {
            int start = (w+w2) * 128;
            for (g = 0; g < ics->num_swb; g++) {
                int p  = -1 + 2 * (cpe->ch[1].band_type[w*16+g] - 14);
                float scale = cpe->ch[0].is_ener[w*16+g];
                if (!cpe->is_mask[w*16 + g]) {
                    start += ics->swb_sizes[g];
                    continue;
                }
                for (i = 0; i < ics->swb_sizes[g]; i++) {
                    float sum = (cpe->ch[0].coeffs[start+i] + p*cpe->ch[1].coeffs[start+i])*scale;
                    cpe->ch[0].coeffs[start+i] = sum;
                    cpe->ch[1].coeffs[start+i] = 0.0f;
                }
                start += ics->swb_sizes[g];
            }
        }
    }
}
