static av_cold int filter_init(AVFilterContext *ctx)
{
    Frei0rContext *s = ctx->priv;
    return frei0r_init(ctx, s->dl_name, F0R_PLUGIN_TYPE_FILTER);
}
