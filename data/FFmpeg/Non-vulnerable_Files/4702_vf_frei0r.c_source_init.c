static av_cold int source_init(AVFilterContext *ctx)
{
    Frei0rContext *s = ctx->priv;
    s->time_base.num = s->framerate.den;
    s->time_base.den = s->framerate.num;
    return frei0r_init(ctx, s->dl_name, F0R_PLUGIN_TYPE_SOURCE);
}
