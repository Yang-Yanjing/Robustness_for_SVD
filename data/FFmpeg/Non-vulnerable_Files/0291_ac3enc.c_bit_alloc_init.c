static av_cold void bit_alloc_init(AC3EncodeContext *s)
{
    int ch;
    
    s->slow_decay_code = 2;
    s->fast_decay_code = 1;
    s->slow_gain_code  = 1;
    s->db_per_bit_code = s->eac3 ? 2 : 3;
    s->floor_code      = 7;
    for (ch = 0; ch <= s->channels; ch++)
        s->fast_gain_code[ch] = 4;
    
    s->coarse_snr_offset = 40;
    
    

    s->bit_alloc.slow_decay = ff_ac3_slow_decay_tab[s->slow_decay_code] >> s->bit_alloc.sr_shift;
    s->bit_alloc.fast_decay = ff_ac3_fast_decay_tab[s->fast_decay_code] >> s->bit_alloc.sr_shift;
    s->bit_alloc.slow_gain  = ff_ac3_slow_gain_tab[s->slow_gain_code];
    s->bit_alloc.db_per_bit = ff_ac3_db_per_bit_tab[s->db_per_bit_code];
    s->bit_alloc.floor      = ff_ac3_floor_tab[s->floor_code];
    s->bit_alloc.cpl_fast_leak = 0;
    s->bit_alloc.cpl_slow_leak = 0;
    count_frame_bits_fixed(s);
}
