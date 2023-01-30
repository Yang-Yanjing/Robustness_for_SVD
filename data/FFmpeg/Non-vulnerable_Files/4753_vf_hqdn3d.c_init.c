static av_cold int init(AVFilterContext *ctx)
{
    HQDN3DContext *s = ctx->priv;
    if (!s->strength[LUMA_SPATIAL])
        s->strength[LUMA_SPATIAL] = PARAM1_DEFAULT;
    if (!s->strength[CHROMA_SPATIAL])
        s->strength[CHROMA_SPATIAL] = PARAM2_DEFAULT * s->strength[LUMA_SPATIAL] / PARAM1_DEFAULT;
    if (!s->strength[LUMA_TMP])
        s->strength[LUMA_TMP]   = PARAM3_DEFAULT * s->strength[LUMA_SPATIAL] / PARAM1_DEFAULT;
    if (!s->strength[CHROMA_TMP])
        s->strength[CHROMA_TMP] = s->strength[LUMA_TMP] * s->strength[CHROMA_SPATIAL] / s->strength[LUMA_SPATIAL];
    av_log(ctx, AV_LOG_VERBOSE, "ls:%f cs:%f lt:%f ct:%f\n",
           s->strength[LUMA_SPATIAL], s->strength[CHROMA_SPATIAL],
           s->strength[LUMA_TMP], s->strength[CHROMA_TMP]);
    return 0;
}
