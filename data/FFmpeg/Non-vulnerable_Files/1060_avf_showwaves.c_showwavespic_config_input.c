AVFILTER_DEFINE_CLASS(showwavespic);
static int showwavespic_config_input(AVFilterLink *inlink)
{
    AVFilterContext *ctx = inlink->dst;
    ShowWavesContext *showwaves = ctx->priv;
    if (showwaves->single_pic) {
        showwaves->sum = av_mallocz_array(inlink->channels, sizeof(*showwaves->sum));
        if (!showwaves->sum)
            return AVERROR(ENOMEM);
    }
    return 0;
}
