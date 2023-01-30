static inline int16_t adpcm_ima_wav_expand_nibble(ADPCMChannelStatus *c, GetBitContext *gb, int bps)
{
    int nibble, step_index, predictor, sign, delta, diff, step, shift;
    shift = bps - 1;
    nibble = get_bits_le(gb, bps),
    step = ff_adpcm_step_table[c->step_index];
    step_index = c->step_index + ff_adpcm_index_tables[bps - 2][nibble];
    step_index = av_clip(step_index, 0, 88);
    sign = nibble & (1 << shift);
    delta = av_mod_uintp2(nibble, shift);
    diff = ((2 * delta + 1) * step) >> shift;
    predictor = c->predictor;
    if (sign) predictor -= diff;
    else predictor += diff;
    c->predictor = av_clip_int16(predictor);
    c->step_index = step_index;
    return (int16_t)c->predictor;
}
