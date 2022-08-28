static int count_exponent_bits(AC3EncodeContext *s)
{
    int blk, ch;
    int nb_groups, bit_count;
    bit_count = 0;
    for (blk = 0; blk < s->num_blocks; blk++) {
        AC3Block *block = &s->blocks[blk];
        for (ch = !block->cpl_in_use; ch <= s->channels; ch++) {
            int exp_strategy = s->exp_strategy[ch][blk];
            int cpl          = (ch == CPL_CH);
            int nb_coefs     = block->end_freq[ch] - s->start_freq[ch];
            if (exp_strategy == EXP_REUSE)
                continue;
            nb_groups = exponent_group_tab[cpl][exp_strategy-1][nb_coefs];
            bit_count += 4 + (nb_groups * 7);
        }
    }
    return bit_count;
}
