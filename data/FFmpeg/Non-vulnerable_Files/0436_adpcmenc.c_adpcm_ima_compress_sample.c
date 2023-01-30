static inline uint8_t adpcm_ima_compress_sample(ADPCMChannelStatus *c,
                                                int16_t sample)
{
    int delta  = sample - c->prev_sample;
    int nibble = FFMIN(7, abs(delta) * 4 /
                       ff_adpcm_step_table[c->step_index]) + (delta < 0) * 8;
    c->prev_sample += ((ff_adpcm_step_table[c->step_index] *
                        ff_adpcm_yamaha_difflookup[nibble]) / 8);
    c->prev_sample = av_clip_int16(c->prev_sample);
    c->step_index  = av_clip(c->step_index + ff_adpcm_index_table[nibble], 0, 88);
    return nibble;
}
