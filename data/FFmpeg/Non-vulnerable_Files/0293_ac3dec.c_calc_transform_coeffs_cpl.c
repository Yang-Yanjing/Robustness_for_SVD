static void calc_transform_coeffs_cpl(AC3DecodeContext *s)
{
    int bin, band, ch;
    bin = s->start_freq[CPL_CH];
    for (band = 0; band < s->num_cpl_bands; band++) {
        int band_start = bin;
        int band_end = bin + s->cpl_band_sizes[band];
        for (ch = 1; ch <= s->fbw_channels; ch++) {
            if (s->channel_in_cpl[ch]) {
                int cpl_coord = s->cpl_coords[ch][band] << 5;
                for (bin = band_start; bin < band_end; bin++) {
                    s->fixed_coeffs[ch][bin] =
                        MULH(s->fixed_coeffs[CPL_CH][bin] * (1 << 4), cpl_coord);
                }
                if (ch == 2 && s->phase_flags[band]) {
                    for (bin = band_start; bin < band_end; bin++)
                        s->fixed_coeffs[2][bin] = -s->fixed_coeffs[2][bin];
                }
            }
        }
        bin = band_end;
    }
}
