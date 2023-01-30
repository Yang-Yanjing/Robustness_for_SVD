static int count_mantissa_bits(AC3EncodeContext *s)
{
    int ch, max_end_freq;
    LOCAL_ALIGNED_16(uint16_t, mant_cnt, [AC3_MAX_BLOCKS], [16]);
    count_mantissa_bits_init(mant_cnt);
    max_end_freq = s->bandwidth_code * 3 + 73;
    for (ch = !s->cpl_enabled; ch <= s->channels; ch++)
        count_mantissa_bits_update_ch(s, ch, mant_cnt, s->start_freq[ch],
                                      max_end_freq);
    return s->ac3dsp.compute_mantissa_size(mant_cnt);
}
