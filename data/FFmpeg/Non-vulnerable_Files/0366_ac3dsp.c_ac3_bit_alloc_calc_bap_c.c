static void ac3_bit_alloc_calc_bap_c(int16_t *mask, int16_t *psd,
                                     int start, int end,
                                     int snr_offset, int floor,
                                     const uint8_t *bap_tab, uint8_t *bap)
{
    int bin, band, band_end;
    
    if (snr_offset == -960) {
        memset(bap, 0, AC3_MAX_COEFS);
        return;
    }
    bin  = start;
    band = ff_ac3_bin_to_band_tab[start];
    do {
        int m = (FFMAX(mask[band] - snr_offset - floor, 0) & 0x1FE0) + floor;
        band_end = ff_ac3_band_start_tab[++band];
        band_end = FFMIN(band_end, end);
        for (; bin < band_end; bin++) {
            int address = av_clip_uintp2((psd[bin] - m) >> 5, 6);
            bap[bin] = bap_tab[address];
        }
    } while (end > band_end);
}
