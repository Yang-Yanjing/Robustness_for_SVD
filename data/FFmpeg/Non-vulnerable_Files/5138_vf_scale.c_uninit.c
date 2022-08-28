static av_cold void uninit(AVFilterContext *ctx)
{
    ScaleContext *scale = ctx->priv;
    sws_freeContext(scale->sws);
    sws_freeContext(scale->isws[0]);
    sws_freeContext(scale->isws[1]);
    scale->sws = NULL;
    av_dict_free(&scale->opts);
}
