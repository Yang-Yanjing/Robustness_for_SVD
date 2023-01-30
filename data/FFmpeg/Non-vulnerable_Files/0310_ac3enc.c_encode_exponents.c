static void encode_exponents(AC3EncodeContext *s)
{
    int blk, blk1, ch, cpl;
    uint8_t *exp, *exp_strategy;
    int nb_coefs, num_reuse_blocks;
    for (ch = !s->cpl_on; ch <= s->channels; ch++) {
        exp          = s->blocks[0].exp[ch] + s->start_freq[ch];
        exp_strategy = s->exp_strategy[ch];
        cpl = (ch == CPL_CH);
        blk = 0;
        while (blk < s->num_blocks) {
            AC3Block *block = &s->blocks[blk];
            if (cpl && !block->cpl_in_use) {
                exp += AC3_MAX_COEFS;
                blk++;
                continue;
            }
            nb_coefs = block->end_freq[ch] - s->start_freq[ch];
            blk1 = blk + 1;
            

            s->exp_ref_block[ch][blk] = blk;
            while (blk1 < s->num_blocks && exp_strategy[blk1] == EXP_REUSE) {
                s->exp_ref_block[ch][blk1] = blk;
                blk1++;
            }
            num_reuse_blocks = blk1 - blk - 1;
            
            s->ac3dsp.ac3_exponent_min(exp-s->start_freq[ch], num_reuse_blocks,
                                       AC3_MAX_COEFS);
            encode_exponents_blk_ch(exp, nb_coefs, exp_strategy[blk], cpl);
            exp += AC3_MAX_COEFS * (num_reuse_blocks + 1);
            blk = blk1;
        }
    }
    
    s->ref_bap_set = 0;
}
