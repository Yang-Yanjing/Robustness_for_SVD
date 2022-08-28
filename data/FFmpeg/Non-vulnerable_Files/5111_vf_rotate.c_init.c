AVFILTER_DEFINE_CLASS(rotate);
static av_cold int init(AVFilterContext *ctx)
{
    RotContext *rot = ctx->priv;
    if (!strcmp(rot->fillcolor_str, "none"))
        rot->fillcolor_enable = 0;
    else if (av_parse_color(rot->fillcolor, rot->fillcolor_str, -1, ctx) >= 0)
        rot->fillcolor_enable = 1;
    else
        return AVERROR(EINVAL);
    return 0;
}
