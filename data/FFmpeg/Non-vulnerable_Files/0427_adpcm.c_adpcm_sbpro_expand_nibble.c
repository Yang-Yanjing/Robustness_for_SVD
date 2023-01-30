static inline short adpcm_sbpro_expand_nibble(ADPCMChannelStatus *c, char nibble, int size, int shift)
{
    int sign, delta, diff;
    sign = nibble & (1<<(size-1));
    delta = nibble & ((1<<(size-1))-1);
    diff = delta << (7 + c->step + shift);
    
    c->predictor = av_clip(c->predictor + (sign ? -diff : diff), -16384,16256);
    
    if (delta >= (2*size - 3) && c->step < 3)
        c->step++;
    else if (delta == 0 && c->step > 0)
        c->step--;
    return (short) c->predictor;
}
