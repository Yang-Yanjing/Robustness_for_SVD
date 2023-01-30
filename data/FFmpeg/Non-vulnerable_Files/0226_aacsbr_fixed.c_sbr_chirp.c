
static void sbr_chirp(SpectralBandReplication *sbr, SBRData *ch_data)
{
    int i;
    int new_bw;
    static const int bw_tab[] = { 0, 1610612736, 1932735283, 2104533975 };
    int64_t accu;
    for (i = 0; i < sbr->n_q; i++) {
        if (ch_data->bs_invf_mode[0][i] + ch_data->bs_invf_mode[1][i] == 1)
            new_bw = 1288490189;
        else
            new_bw = bw_tab[ch_data->bs_invf_mode[0][i]];
        if (new_bw < ch_data->bw_array[i]){
            accu  = (int64_t)new_bw * 1610612736;
            accu += (int64_t)ch_data->bw_array[i] * 0x20000000;
            new_bw = (int)((accu + 0x40000000) >> 31);
        } else {
            accu  = (int64_t)new_bw * 1946157056;
            accu += (int64_t)ch_data->bw_array[i] * 201326592;
            new_bw = (int)((accu + 0x40000000) >> 31);
        }
        ch_data->bw_array[i] = new_bw < 0x2000000 ? 0 : new_bw;
    }
}
