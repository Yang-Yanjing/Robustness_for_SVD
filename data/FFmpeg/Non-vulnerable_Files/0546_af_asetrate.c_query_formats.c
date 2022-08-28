AVFILTER_DEFINE_CLASS(asetrate);
static av_cold int query_formats(AVFilterContext *ctx)
{
    ASetRateContext *sr = ctx->priv;
    int sample_rates[] = { sr->sample_rate, -1 };
    return ff_formats_ref(ff_make_format_list(sample_rates),
                   &ctx->outputs[0]->in_samplerates);
}
