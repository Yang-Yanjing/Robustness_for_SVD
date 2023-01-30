static void count_mantissa_bits_update_ch(AC3EncodeContext *s, int ch,
                                          uint16_t mant_cnt[AC3_MAX_BLOCKS][16],
                                          int start, int end)
{
    int blk;
    for (blk = 0; blk < s->num_blocks; blk++) {
        AC3Block *block = &s->blocks[blk];
        if (ch == CPL_CH && !block->cpl_in_use)
            continue;
        s->ac3dsp.update_bap_counts(mant_cnt[blk],
                                    s->ref_bap[ch][blk] + start,
                                    FFMIN(end, block->end_freq[ch]) - start);
    }
}
