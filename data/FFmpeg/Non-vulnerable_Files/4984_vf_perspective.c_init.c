static av_cold int init(AVFilterContext *ctx)
{
    PerspectiveContext *s = ctx->priv;
    switch (s->interpolation) {
    case LINEAR: s->perspective = resample_linear; break;
    case CUBIC:  s->perspective = resample_cubic;  break;
    }
    return 0;
}
