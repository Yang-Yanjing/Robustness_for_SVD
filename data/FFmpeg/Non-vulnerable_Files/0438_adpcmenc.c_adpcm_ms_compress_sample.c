static inline uint8_t adpcm_ms_compress_sample(ADPCMChannelStatus *c,
                                               int16_t sample)
{
    int predictor, nibble, bias;
    predictor = (((c->sample1) * (c->coeff1)) +
                (( c->sample2) * (c->coeff2))) / 64;
    nibble = sample - predictor;
    if (nibble >= 0)
        bias =  c->idelta / 2;
    else
        bias = -c->idelta / 2;
    nibble = (nibble + bias) / c->idelta;
    nibble = av_clip_intp2(nibble, 3) & 0x0F;
    predictor += ((nibble & 0x08) ? (nibble - 0x10) : nibble) * c->idelta;
    c->sample2 = c->sample1;
    c->sample1 = av_clip_int16(predictor);
    c->idelta = (ff_adpcm_AdaptationTable[nibble] * c->idelta) >> 8;
    if (c->idelta < 16)
        c->idelta = 16;
    return nibble;
}
