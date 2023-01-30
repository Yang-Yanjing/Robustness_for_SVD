static av_cold void uninit(AVFilterContext *ctx)
{
    Frei0rContext *s = ctx->priv;
    if (s->destruct && s->instance)
        s->destruct(s->instance);
    if (s->deinit)
        s->deinit();
    if (s->dl_handle)
        dlclose(s->dl_handle);
}
