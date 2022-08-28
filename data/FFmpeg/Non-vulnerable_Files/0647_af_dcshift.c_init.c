AVFILTER_DEFINE_CLASS(dcshift);
static av_cold int init(AVFilterContext *ctx)
{
    DCShiftContext *s = ctx->priv;
    s->limiterthreshhold = INT32_MAX * (1.0 - (fabs(s->dcshift) - s->limitergain));
    return 0;
}
