
static void read_sbr_dtdf(SpectralBandReplication *sbr, GetBitContext *gb,
                          SBRData *ch_data)
{
    get_bits1_vector(gb, ch_data->bs_df_env,   ch_data->bs_num_env);
    get_bits1_vector(gb, ch_data->bs_df_noise, ch_data->bs_num_noise);
}
