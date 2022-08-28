av_cold void ff_ac3_common_init(void)
{
#if !CONFIG_HARDCODED_TABLES
    
    int bin = 0, band;
    for (band = 0; band < AC3_CRITICAL_BANDS; band++) {
        int band_end = ff_ac3_band_start_tab[band+1];
        while (bin < band_end)
            ff_ac3_bin_to_band_tab[bin++] = band;
    }
#endif 
}
