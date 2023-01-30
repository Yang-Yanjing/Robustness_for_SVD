AVFILTER_DEFINE_CLASS(signalstats);
static av_cold int init(AVFilterContext *ctx)
{
    uint8_t r, g, b;
    SignalstatsContext *s = ctx->priv;
    if (s->outfilter != FILTER_NONE)
        s->filters |= 1 << s->outfilter;
    r = s->rgba_color[0];
    g = s->rgba_color[1];
    b = s->rgba_color[2];
    s->yuv_color[0] = (( 66*r + 129*g +  25*b + (1<<7)) >> 8) +  16;
    s->yuv_color[1] = ((-38*r + -74*g + 112*b + (1<<7)) >> 8) + 128;
    s->yuv_color[2] = ((112*r + -94*g + -18*b + (1<<7)) >> 8) + 128;
    return 0;
}
