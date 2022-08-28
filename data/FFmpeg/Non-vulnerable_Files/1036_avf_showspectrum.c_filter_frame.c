static int filter_frame(AVFilterLink *inlink, AVFrame *insamples)
{
    AVFilterContext *ctx = inlink->dst;
    ShowSpectrumContext *s = ctx->priv;
    unsigned win_size = 1 << s->rdft_bits;
    int ret = 0;
    av_assert0(insamples->nb_samples <= win_size);
    if (insamples->nb_samples == win_size)
        ret = plot_spectrum_column(inlink, insamples);
    av_frame_free(&insamples);
    return ret;
}
