SILENCE_DETECT(s16, int16_t)
static int config_input(AVFilterLink *inlink)
{
    AVFilterContext *ctx = inlink->dst;
    SilenceDetectContext *s = ctx->priv;
    switch (inlink->format) {
    case AV_SAMPLE_FMT_DBL: s->silencedetect = silencedetect_dbl; break;
    case AV_SAMPLE_FMT_FLT: s->silencedetect = silencedetect_flt; break;
    case AV_SAMPLE_FMT_S32:
        s->noise *= INT32_MAX;
        s->silencedetect = silencedetect_s32;
        break;
    case AV_SAMPLE_FMT_S16:
        s->noise *= INT16_MAX;
        s->silencedetect = silencedetect_s16;
        break;
    }
    return 0;
}
