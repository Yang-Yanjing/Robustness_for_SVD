AVFILTER_DEFINE_CLASS(asetnsamples);
static av_cold int init(AVFilterContext *ctx)
{
    ASNSContext *asns = ctx->priv;
    asns->next_out_pts = AV_NOPTS_VALUE;
    av_log(ctx, AV_LOG_VERBOSE, "nb_out_samples:%d pad:%d\n", asns->nb_out_samples, asns->pad);
    return 0;
}
