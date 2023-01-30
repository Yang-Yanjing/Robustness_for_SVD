AVFILTER_DEFINE_CLASS(sab);
static av_cold int init(AVFilterContext *ctx)
{
    SabContext *s = ctx->priv;
    
    if (s->chroma.radius < RADIUS_MIN)
        s->chroma.radius = s->luma.radius;
    if (s->chroma.pre_filter_radius < PRE_FILTER_RADIUS_MIN)
        s->chroma.pre_filter_radius = s->luma.pre_filter_radius;
    if (s->chroma.strength < STRENGTH_MIN)
        s->chroma.strength = s->luma.strength;
    s->luma.quality = s->chroma.quality = 3.0;
    s->sws_flags = SWS_POINT;
    av_log(ctx, AV_LOG_VERBOSE,
           "luma_radius:%f luma_pre_filter_radius::%f luma_strength:%f "
           "chroma_radius:%f chroma_pre_filter_radius:%f chroma_strength:%f\n",
           s->luma  .radius, s->luma  .pre_filter_radius, s->luma  .strength,
           s->chroma.radius, s->chroma.pre_filter_radius, s->chroma.strength);
    return 0;
}
