
static void read_sbr_invf(SpectralBandReplication *sbr, GetBitContext *gb,
                          SBRData *ch_data)
{
    int i;
    memcpy(ch_data->bs_invf_mode[1], ch_data->bs_invf_mode[0], 5 * sizeof(uint8_t));
    for (i = 0; i < sbr->n_q; i++)
        ch_data->bs_invf_mode[0][i] = get_bits(gb, 2);
}
