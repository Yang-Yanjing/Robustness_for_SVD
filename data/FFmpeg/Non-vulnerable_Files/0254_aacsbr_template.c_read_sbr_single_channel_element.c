static int read_sbr_single_channel_element(AACContext *ac,
                                            SpectralBandReplication *sbr,
                                            GetBitContext *gb)
{
    if (get_bits1(gb)) 
        skip_bits(gb, 4); 
    if (read_sbr_grid(ac, sbr, gb, &sbr->data[0]))
        return -1;
    read_sbr_dtdf(sbr, gb, &sbr->data[0]);
    read_sbr_invf(sbr, gb, &sbr->data[0]);
    read_sbr_envelope(sbr, gb, &sbr->data[0], 0);
    read_sbr_noise(sbr, gb, &sbr->data[0], 0);
    if ((sbr->data[0].bs_add_harmonic_flag = get_bits1(gb)))
        get_bits1_vector(gb, sbr->data[0].bs_add_harmonic, sbr->n[1]);
    return 0;
}
