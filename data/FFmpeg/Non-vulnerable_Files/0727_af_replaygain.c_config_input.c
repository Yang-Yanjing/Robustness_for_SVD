static int config_input(AVFilterLink *inlink)
{
    AVFilterContext *ctx = inlink->dst;
    ReplayGainContext *s = ctx->priv;
    int i;
    for (i = 0; i < FF_ARRAY_ELEMS(freqinfos); i++) {
        if (freqinfos[i].sample_rate == inlink->sample_rate)
            break;
    }
    av_assert0(i < FF_ARRAY_ELEMS(freqinfos));
    s->yule_coeff_a   = freqinfos[i].AYule;
    s->yule_coeff_b   = freqinfos[i].BYule;
    s->butter_coeff_a = freqinfos[i].AButter;
    s->butter_coeff_b = freqinfos[i].BButter;
    s->yule_hist_i   = 20;
    s->butter_hist_i = 4;
    inlink->partial_buf_size =
    inlink->min_samples =
    inlink->max_samples = inlink->sample_rate / 20;
    return 0;
}
