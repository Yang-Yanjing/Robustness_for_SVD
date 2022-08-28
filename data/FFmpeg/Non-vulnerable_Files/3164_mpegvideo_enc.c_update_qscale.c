static inline void update_qscale(MpegEncContext *s)
{
    if (s->q_scale_type == 1) {
        int i;
        int bestdiff=INT_MAX;
        int best = 1;
        static const uint8_t non_linear_qscale[] = {
            1,2,3,4,5,6,7,8,9,10,11,12,14,16,18,20,24,26,28
        };
        for (i = 0 ; i<FF_ARRAY_ELEMS(non_linear_qscale); i++) {
            int diff = FFABS((non_linear_qscale[i]<<(FF_LAMBDA_SHIFT + 7)) - (int)s->lambda * 139);
            if (non_linear_qscale[i] < s->avctx->qmin ||
                (non_linear_qscale[i] > s->avctx->qmax && !s->vbv_ignore_qmax))
                continue;
            if (diff < bestdiff) {
                bestdiff = diff;
                best = non_linear_qscale[i];
            }
        }
        s->qscale = best;
    } else {
        s->qscale = (s->lambda * 139 + FF_LAMBDA_SCALE * 64) >>
                    (FF_LAMBDA_SHIFT + 7);
        s->qscale = av_clip(s->qscale, s->avctx->qmin, s->vbv_ignore_qmax ? 31 : s->avctx->qmax);
    }
    s->lambda2 = (s->lambda * s->lambda + FF_LAMBDA_SCALE / 2) >>
                 FF_LAMBDA_SHIFT;
}
