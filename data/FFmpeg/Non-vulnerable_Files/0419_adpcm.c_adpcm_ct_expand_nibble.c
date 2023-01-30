static inline short adpcm_ct_expand_nibble(ADPCMChannelStatus *c, char nibble)
{
    int sign, delta, diff;
    int new_step;
    sign = nibble & 8;
    delta = nibble & 7;
    


    diff = ((2 * delta + 1) * c->step) >> 3;
    
    c->predictor = ((c->predictor * 254) >> 8) + (sign ? -diff : diff);
    c->predictor = av_clip_int16(c->predictor);
    
    new_step = (ff_adpcm_AdaptationTable[nibble & 7] * c->step) >> 8;
    c->step = av_clip(new_step, 511, 32767);
    return (short)c->predictor;
}
