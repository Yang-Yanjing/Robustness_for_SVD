static void remove_dithering(AC3DecodeContext *s) {
    int ch, i;
    for (ch = 1; ch <= s->fbw_channels; ch++) {
        if (!s->dither_flag[ch] && s->channel_in_cpl[ch]) {
            for (i = s->start_freq[CPL_CH]; i < s->end_freq[CPL_CH]; i++) {
                if (!s->bap[CPL_CH][i])
                    s->fixed_coeffs[ch][i] = 0;
            }
        }
    }
}
