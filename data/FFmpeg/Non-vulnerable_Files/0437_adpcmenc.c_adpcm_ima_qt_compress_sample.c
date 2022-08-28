static inline uint8_t adpcm_ima_qt_compress_sample(ADPCMChannelStatus *c,
                                                   int16_t sample)
{
    int delta  = sample - c->prev_sample;
    int diff, step = ff_adpcm_step_table[c->step_index];
    int nibble = 8*(delta < 0);
    delta= abs(delta);
    diff = delta + (step >> 3);
    if (delta >= step) {
        nibble |= 4;
        delta  -= step;
    }
    step >>= 1;
    if (delta >= step) {
        nibble |= 2;
        delta  -= step;
    }
    step >>= 1;
    if (delta >= step) {
        nibble |= 1;
        delta  -= step;
    }
    diff -= delta;
    if (nibble & 8)
        c->prev_sample -= diff;
    else
        c->prev_sample += diff;
    c->prev_sample = av_clip_int16(c->prev_sample);
    c->step_index  = av_clip(c->step_index + ff_adpcm_index_table[nibble], 0, 88);
    return nibble;
}
