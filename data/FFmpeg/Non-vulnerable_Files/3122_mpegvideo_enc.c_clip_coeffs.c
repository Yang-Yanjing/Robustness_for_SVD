static inline void clip_coeffs(MpegEncContext *s, int16_t *block,
                               int last_index)
{
    int i;
    const int maxlevel = s->max_qcoeff;
    const int minlevel = s->min_qcoeff;
    int overflow = 0;
    if (s->mb_intra) {
        i = 1; 
    } else
        i = 0;
    for (; i <= last_index; i++) {
        const int j = s->intra_scantable.permutated[i];
        int level = block[j];
        if (level > maxlevel) {
            level = maxlevel;
            overflow++;
        } else if (level < minlevel) {
            level = minlevel;
            overflow++;
        }
        block[j] = level;
    }
    if (overflow && s->avctx->mb_decision == FF_MB_DECISION_SIMPLE)
        av_log(s->avctx, AV_LOG_INFO,
               "warning, clipping %d dct coefficients to %d..%d\n",
               overflow, minlevel, maxlevel);
}
