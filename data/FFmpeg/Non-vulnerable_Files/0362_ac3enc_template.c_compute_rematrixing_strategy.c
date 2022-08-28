static void compute_rematrixing_strategy(AC3EncodeContext *s)
{
    int nb_coefs;
    int blk, bnd;
    AC3Block *block, *block0 = NULL;
    if (s->channel_mode != AC3_CHMODE_STEREO)
        return;
    for (blk = 0; blk < s->num_blocks; blk++) {
        block = &s->blocks[blk];
        block->new_rematrixing_strategy = !blk;
        block->num_rematrixing_bands = 4;
        if (block->cpl_in_use) {
            block->num_rematrixing_bands -= (s->start_freq[CPL_CH] <= 61);
            block->num_rematrixing_bands -= (s->start_freq[CPL_CH] == 37);
            if (blk && block->num_rematrixing_bands != block0->num_rematrixing_bands)
                block->new_rematrixing_strategy = 1;
        }
        nb_coefs = FFMIN(block->end_freq[1], block->end_freq[2]);
        if (!s->rematrixing_enabled) {
            block0 = block;
            continue;
        }
        for (bnd = 0; bnd < block->num_rematrixing_bands; bnd++) {
            
            int start = ff_ac3_rematrix_band_tab[bnd];
            int end   = FFMIN(nb_coefs, ff_ac3_rematrix_band_tab[bnd+1]);
            CoefSumType sum[4];
            sum_square_butterfly(s, sum, block->mdct_coef[1] + start,
                                 block->mdct_coef[2] + start, end - start);
            
            if (FFMIN(sum[2], sum[3]) < FFMIN(sum[0], sum[1]))
                block->rematrixing_flags[bnd] = 1;
            else
                block->rematrixing_flags[bnd] = 0;
            
            if (blk &&
                block->rematrixing_flags[bnd] != block0->rematrixing_flags[bnd]) {
                block->new_rematrixing_strategy = 1;
            }
        }
        block0 = block;
    }
}
