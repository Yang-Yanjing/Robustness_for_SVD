static inline void dct_single_coeff_elimination(MpegEncContext *s,
                                                int n, int threshold)
{
    static const char tab[64] = {
        3, 2, 2, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0
    };
    int score = 0;
    int run = 0;
    int i;
    int16_t *block = s->block[n];
    const int last_index = s->block_last_index[n];
    int skip_dc;
    if (threshold < 0) {
        skip_dc = 0;
        threshold = -threshold;
    } else
        skip_dc = 1;
    
    if (last_index <= skip_dc - 1)
        return;
    for (i = 0; i <= last_index; i++) {
        const int j = s->intra_scantable.permutated[i];
        const int level = FFABS(block[j]);
        if (level == 1) {
            if (skip_dc && i == 0)
                continue;
            score += tab[run];
            run = 0;
        } else if (level > 1) {
            return;
        } else {
            run++;
        }
    }
    if (score >= threshold)
        return;
    for (i = skip_dc; i <= last_index; i++) {
        const int j = s->intra_scantable.permutated[i];
        block[j] = 0;
    }
    if (block[0])
        s->block_last_index[n] = 0;
    else
        s->block_last_index[n] = -1;
}
