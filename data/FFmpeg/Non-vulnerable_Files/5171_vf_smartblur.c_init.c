AVFILTER_DEFINE_CLASS(smartblur);
static av_cold int init(AVFilterContext *ctx)
{
    SmartblurContext *s = ctx->priv;
    
    if (s->chroma.radius < RADIUS_MIN)
        s->chroma.radius = s->luma.radius;
    if (s->chroma.strength < STRENGTH_MIN)
        s->chroma.strength  = s->luma.strength;
    if (s->chroma.threshold < THRESHOLD_MIN)
        s->chroma.threshold = s->luma.threshold;
    s->luma.quality = s->chroma.quality = 3.0;
    s->sws_flags = SWS_BICUBIC;
    av_log(ctx, AV_LOG_VERBOSE,
           "luma_radius:%f luma_strength:%f luma_threshold:%d "
           "chroma_radius:%f chroma_strength:%f chroma_threshold:%d\n",
           s->luma.radius, s->luma.strength, s->luma.threshold,
           s->chroma.radius, s->chroma.strength, s->chroma.threshold);
    return 0;
}
