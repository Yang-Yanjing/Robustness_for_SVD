int16_t ff_acelp_decode_gain_code(
    AudioDSPContext *adsp,
    int gain_corr_factor,
    const int16_t* fc_v,
    int mr_energy,
    const int16_t* quant_energy,
    const int16_t* ma_prediction_coeff,
    int subframe_size,
    int ma_pred_order)
{
    int i;

    mr_energy <<= 10;

    for(i=0; i<ma_pred_order; i++)
        mr_energy += quant_energy[i] * ma_prediction_coeff[i];

#ifdef G729_BITEXACT
    mr_energy += (((-6165LL * ff_log2(dsp->scalarproduct_int16(fc_v, fc_v, subframe_size, 0))) >> 3) & ~0x3ff);

    mr_energy = (5439 * (mr_energy >> 15)) >> 8;           

    return bidir_sal(
               ((ff_exp2(mr_energy & 0x7fff) + 16) >> 5) * (gain_corr_factor >> 1),
               (mr_energy >> 15) - 25
           );
#else
    mr_energy = gain_corr_factor * exp(M_LN10 / (20 << 23) * mr_energy) /
                sqrt(adsp->scalarproduct_int16(fc_v, fc_v, subframe_size));
    return mr_energy >> 12;
#endif
}