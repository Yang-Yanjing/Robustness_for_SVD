static av_cold int init(AVFilterContext *ctx)
{
    ATempoContext *atempo = ctx->priv;
    atempo->format = AV_SAMPLE_FMT_NONE;
    atempo->state  = YAE_LOAD_FRAGMENT;
    return 0;
}
