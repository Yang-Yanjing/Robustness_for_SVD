static inline short adpcm_ima_oki_expand_nibble(ADPCMChannelStatus *c, int nibble)
{
    int step_index, predictor, sign, delta, diff, step;
    step = ff_adpcm_oki_step_table[c->step_index];
    step_index = c->step_index + ff_adpcm_index_table[(unsigned)nibble];
    step_index = av_clip(step_index, 0, 48);
    sign = nibble & 8;
    delta = nibble & 7;
    diff = ((2 * delta + 1) * step) >> 3;
    predictor = c->predictor;
    if (sign) predictor -= diff;
    else predictor += diff;
    c->predictor = av_clip_intp2(predictor, 11);
    c->step_index = step_index;
    return c->predictor << 4;
}
