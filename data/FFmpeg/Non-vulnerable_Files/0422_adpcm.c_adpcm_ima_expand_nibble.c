static inline short adpcm_ima_expand_nibble(ADPCMChannelStatus *c, char nibble, int shift)
{
    int step_index;
    int predictor;
    int sign, delta, diff, step;
    step = ff_adpcm_step_table[c->step_index];
    step_index = c->step_index + ff_adpcm_index_table[(unsigned)nibble];
    step_index = av_clip(step_index, 0, 88);
    sign = nibble & 8;
    delta = nibble & 7;
    


    diff = ((2 * delta + 1) * step) >> shift;
    predictor = c->predictor;
    if (sign) predictor -= diff;
    else predictor += diff;
    c->predictor = av_clip_int16(predictor);
    c->step_index = step_index;
    return (short)c->predictor;
}
