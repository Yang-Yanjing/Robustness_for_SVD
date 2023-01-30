static void reset_block_bap(AC3EncodeContext *s)
{
    int blk, ch;
    uint8_t *ref_bap;
    if (s->ref_bap[0][0] == s->bap_buffer && s->ref_bap_set)
        return;
    ref_bap = s->bap_buffer;
    for (ch = 0; ch <= s->channels; ch++) {
        for (blk = 0; blk < s->num_blocks; blk++)
            s->ref_bap[ch][blk] = ref_bap + AC3_MAX_COEFS * s->exp_ref_block[ch][blk];
        ref_bap += AC3_MAX_COEFS * s->num_blocks;
    }
    s->ref_bap_set = 1;
}
