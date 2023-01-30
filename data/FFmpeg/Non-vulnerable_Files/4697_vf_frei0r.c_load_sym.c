static void *load_sym(AVFilterContext *ctx, const char *sym_name)
{
    Frei0rContext *s = ctx->priv;
    void *sym = dlsym(s->dl_handle, sym_name);
    if (!sym)
        av_log(ctx, AV_LOG_ERROR, "Could not find symbol '%s' in loaded module.\n", sym_name);
    return sym;
}
