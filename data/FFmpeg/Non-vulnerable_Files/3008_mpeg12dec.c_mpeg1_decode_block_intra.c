static inline int mpeg1_decode_block_intra(MpegEncContext *s,
                                           int16_t *block, int n)
{
    int level, dc, diff, i, j, run;
    int component;
    RLTable *rl                  = &ff_rl_mpeg1;
    uint8_t *const scantable     = s->intra_scantable.permutated;
    const uint16_t *quant_matrix = s->intra_matrix;
    const int qscale             = s->qscale;
    
    component = (n <= 3 ? 0 : n - 4 + 1);
    diff = decode_dc(&s->gb, component);
    if (diff >= 0xffff)
        return AVERROR_INVALIDDATA;
    dc  = s->last_dc[component];
    dc += diff;
    s->last_dc[component] = dc;
    block[0] = dc * quant_matrix[0];
    ff_tlog(s->avctx, "dc=%d diff=%d\n", dc, diff);
    i = 0;
    {
        OPEN_READER(re, &s->gb);
        UPDATE_CACHE(re, &s->gb);
        if (((int32_t)GET_CACHE(re, &s->gb)) <= (int32_t)0xBFFFFFFF)
            goto end;
        
        for (;;) {
            GET_RL_VLC(level, run, re, &s->gb, rl->rl_vlc[0],
                       TEX_VLC_BITS, 2, 0);
            if (level != 0) {
                i += run;
                check_scantable_index(s, i);
                j = scantable[i];
                level = (level * qscale * quant_matrix[j]) >> 4;
                level = (level - 1) | 1;
                level = (level ^ SHOW_SBITS(re, &s->gb, 1)) -
                        SHOW_SBITS(re, &s->gb, 1);
                SKIP_BITS(re, &s->gb, 1);
            } else {
                
                run = SHOW_UBITS(re, &s->gb, 6) + 1;
                LAST_SKIP_BITS(re, &s->gb, 6);
                UPDATE_CACHE(re, &s->gb);
                level = SHOW_SBITS(re, &s->gb, 8);
                SKIP_BITS(re, &s->gb, 8);
                if (level == -128) {
                    level = SHOW_UBITS(re, &s->gb, 8) - 256;
                    SKIP_BITS(re, &s->gb, 8);
                } else if (level == 0) {
                    level = SHOW_UBITS(re, &s->gb, 8);
                    SKIP_BITS(re, &s->gb, 8);
                }
                i += run;
                check_scantable_index(s, i);
                j = scantable[i];
                if (level < 0) {
                    level = -level;
                    level = (level * qscale * quant_matrix[j]) >> 4;
                    level = (level - 1) | 1;
                    level = -level;
                } else {
                    level = (level * qscale * quant_matrix[j]) >> 4;
                    level = (level - 1) | 1;
                }
            }
            block[j] = level;
            if (((int32_t)GET_CACHE(re, &s->gb)) <= (int32_t)0xBFFFFFFF)
               break;
            UPDATE_CACHE(re, &s->gb);
        }
end:
        LAST_SKIP_BITS(re, &s->gb, 2);
        CLOSE_READER(re, &s->gb);
    }
    s->block_last_index[n] = i;
    return 0;
}
