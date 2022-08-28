static unsigned int read_sbr_header(SpectralBandReplication *sbr, GetBitContext *gb)
{
    unsigned int cnt = get_bits_count(gb);
    uint8_t bs_header_extra_1;
    uint8_t bs_header_extra_2;
    int old_bs_limiter_bands = sbr->bs_limiter_bands;
    SpectrumParameters old_spectrum_params;
    sbr->start = 1;
    
    memcpy(&old_spectrum_params, &sbr->spectrum_params, sizeof(SpectrumParameters));
    sbr->bs_amp_res_header              = get_bits1(gb);
    sbr->spectrum_params.bs_start_freq  = get_bits(gb, 4);
    sbr->spectrum_params.bs_stop_freq   = get_bits(gb, 4);
    sbr->spectrum_params.bs_xover_band  = get_bits(gb, 3);
                                          skip_bits(gb, 2); 
    bs_header_extra_1 = get_bits1(gb);
    bs_header_extra_2 = get_bits1(gb);
    if (bs_header_extra_1) {
        sbr->spectrum_params.bs_freq_scale  = get_bits(gb, 2);
        sbr->spectrum_params.bs_alter_scale = get_bits1(gb);
        sbr->spectrum_params.bs_noise_bands = get_bits(gb, 2);
    } else {
        sbr->spectrum_params.bs_freq_scale  = 2;
        sbr->spectrum_params.bs_alter_scale = 1;
        sbr->spectrum_params.bs_noise_bands = 2;
    }
    
    if (memcmp(&old_spectrum_params, &sbr->spectrum_params, sizeof(SpectrumParameters)))
        sbr->reset = 1;
    if (bs_header_extra_2) {
        sbr->bs_limiter_bands  = get_bits(gb, 2);
        sbr->bs_limiter_gains  = get_bits(gb, 2);
        sbr->bs_interpol_freq  = get_bits1(gb);
        sbr->bs_smoothing_mode = get_bits1(gb);
    } else {
        sbr->bs_limiter_bands  = 2;
        sbr->bs_limiter_gains  = 2;
        sbr->bs_interpol_freq  = 1;
        sbr->bs_smoothing_mode = 1;
    }
    if (sbr->bs_limiter_bands != old_bs_limiter_bands && !sbr->reset)
        sbr_make_f_tablelim(sbr);
    return get_bits_count(gb) - cnt;
}
