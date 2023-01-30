void ff_ac3_quantize_mantissas(AC3EncodeContext *s)
{
    int blk, ch, ch0=0, got_cpl;
    for (blk = 0; blk < s->num_blocks; blk++) {
        AC3Block *block = &s->blocks[blk];
        AC3Mant m = { 0 };
        got_cpl = !block->cpl_in_use;
        for (ch = 1; ch <= s->channels; ch++) {
            if (!got_cpl && ch > 1 && block->channel_in_cpl[ch-1]) {
                ch0     = ch - 1;
                ch      = CPL_CH;
                got_cpl = 1;
            }
            quantize_mantissas_blk_ch(&m, block->fixed_coef[ch],
                                      s->blocks[s->exp_ref_block[ch][blk]].exp[ch],
                                      s->ref_bap[ch][blk], block->qmant[ch],
                                      s->start_freq[ch], block->end_freq[ch]);
            if (ch == CPL_CH)
                ch = ch0;
        }
    }
}
