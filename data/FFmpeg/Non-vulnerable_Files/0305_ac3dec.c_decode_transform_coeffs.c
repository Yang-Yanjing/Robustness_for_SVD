static void decode_transform_coeffs(AC3DecodeContext *s, int blk)
{
    int ch, end;
    int got_cplchan = 0;
    mant_groups m;
    m.b1 = m.b2 = m.b4 = 0;
    for (ch = 1; ch <= s->channels; ch++) {
        
        decode_transform_coeffs_ch(s, blk, ch, &m);
        

        if (s->channel_in_cpl[ch])  {
            if (!got_cplchan) {
                decode_transform_coeffs_ch(s, blk, CPL_CH, &m);
                calc_transform_coeffs_cpl(s);
                got_cplchan = 1;
            }
            end = s->end_freq[CPL_CH];
        } else {
            end = s->end_freq[ch];
        }
        do
            s->fixed_coeffs[ch][end] = 0;
        while (++end < 256);
    }
    
    remove_dithering(s);
}
