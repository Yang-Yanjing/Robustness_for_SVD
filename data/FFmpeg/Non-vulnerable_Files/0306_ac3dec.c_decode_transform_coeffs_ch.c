static void decode_transform_coeffs_ch(AC3DecodeContext *s, int blk, int ch,
                                       mant_groups *m)
{
    if (!s->channel_uses_aht[ch]) {
        ac3_decode_transform_coeffs_ch(s, ch, m);
    } else {
        

        int bin;
        if (CONFIG_EAC3_DECODER && !blk)
            ff_eac3_decode_transform_coeffs_aht_ch(s, ch);
        for (bin = s->start_freq[ch]; bin < s->end_freq[ch]; bin++) {
            s->fixed_coeffs[ch][bin] = s->pre_mantissa[ch][bin][blk] >> s->dexps[ch][bin];
        }
    }
}
