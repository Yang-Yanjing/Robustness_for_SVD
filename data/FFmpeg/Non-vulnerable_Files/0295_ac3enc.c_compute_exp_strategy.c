static void compute_exp_strategy(AC3EncodeContext *s)
{
    int ch, blk, blk1;
    for (ch = !s->cpl_on; ch <= s->fbw_channels; ch++) {
        uint8_t *exp_strategy = s->exp_strategy[ch];
        uint8_t *exp          = s->blocks[0].exp[ch];
        int exp_diff;
        

        exp_strategy[0] = EXP_NEW;
        exp += AC3_MAX_COEFS;
        for (blk = 1; blk < s->num_blocks; blk++, exp += AC3_MAX_COEFS) {
            if (ch == CPL_CH) {
                if (!s->blocks[blk-1].cpl_in_use) {
                    exp_strategy[blk] = EXP_NEW;
                    continue;
                } else if (!s->blocks[blk].cpl_in_use) {
                    exp_strategy[blk] = EXP_REUSE;
                    continue;
                }
            } else if (s->blocks[blk].channel_in_cpl[ch] != s->blocks[blk-1].channel_in_cpl[ch]) {
                exp_strategy[blk] = EXP_NEW;
                continue;
            }
            exp_diff = s->mecc.sad[0](NULL, exp, exp - AC3_MAX_COEFS, 16, 16);
            exp_strategy[blk] = EXP_REUSE;
            if (ch == CPL_CH && exp_diff > (EXP_DIFF_THRESHOLD * (s->blocks[blk].end_freq[ch] - s->start_freq[ch]) / AC3_MAX_COEFS))
                exp_strategy[blk] = EXP_NEW;
            else if (ch > CPL_CH && exp_diff > EXP_DIFF_THRESHOLD)
                exp_strategy[blk] = EXP_NEW;
        }
        

        blk = 0;
        while (blk < s->num_blocks) {
            blk1 = blk + 1;
            while (blk1 < s->num_blocks && exp_strategy[blk1] == EXP_REUSE)
                blk1++;
            exp_strategy[blk] = exp_strategy_reuse_tab[s->num_blks_code][blk1-blk-1];
            blk = blk1;
        }
    }
    if (s->lfe_on) {
        ch = s->lfe_channel;
        s->exp_strategy[ch][0] = EXP_D15;
        for (blk = 1; blk < s->num_blocks; blk++)
            s->exp_strategy[ch][blk] = EXP_REUSE;
    }
    
    if (CONFIG_EAC3_ENCODER && s->eac3)
        ff_eac3_get_frame_exp_strategy(s);
}
