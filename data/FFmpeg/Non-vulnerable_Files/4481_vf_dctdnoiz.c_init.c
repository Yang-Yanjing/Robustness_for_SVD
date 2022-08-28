static av_cold int init(AVFilterContext *ctx)
{
    DCTdnoizContext *s = ctx->priv;
    s->bsize = 1 << s->n;
    if (s->overlap == -1)
        s->overlap = s->bsize - 1;
    if (s->overlap > s->bsize - 1) {
        av_log(s, AV_LOG_ERROR, "Overlap value can not except %d "
               "with a block size of %dx%d\n",
               s->bsize - 1, s->bsize, s->bsize);
        return AVERROR(EINVAL);
    }
    if (s->expr_str) {
        switch (s->n) {
        case 3: s->filter_freq_func = filter_freq_expr_8;  break;
        case 4: s->filter_freq_func = filter_freq_expr_16; break;
        default: av_assert0(0);
        }
    } else {
        switch (s->n) {
        case 3: s->filter_freq_func = filter_freq_sigma_8;  break;
        case 4: s->filter_freq_func = filter_freq_sigma_16; break;
        default: av_assert0(0);
        }
    }
    s->th   = s->sigma * 3.;
    s->step = s->bsize - s->overlap;
    return 0;
}
