void ff_acelp_update_past_gain(
    int16_t* quant_energy,
    int gain_corr_factor,
    int log2_ma_pred_order,
    int erasure)
{
    int i;
    int avg_gain=quant_energy[(1 << log2_ma_pred_order) - 1]; 
    for(i=(1 << log2_ma_pred_order) - 1; i>0; i--)
    {
        avg_gain       += quant_energy[i-1];
        quant_energy[i] = quant_energy[i-1];
    }
    if(erasure)
        quant_energy[0] = FFMAX(avg_gain >> log2_ma_pred_order, -10240) - 4096; 
    else
        quant_energy[0] = (6165 * ((ff_log2_q15(gain_corr_factor) >> 2) - (13 << 13))) >> 13;
}
