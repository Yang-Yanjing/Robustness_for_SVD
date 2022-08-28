static int load_font(AVFilterContext *ctx)
{
    DrawTextContext *s = ctx->priv;
    int err;
    
    err = load_font_file(ctx, s->fontfile, 0);
    if (!err)
        return 0;
#if CONFIG_LIBFONTCONFIG
    err = load_font_fontconfig(ctx);
    if (!err)
        return 0;
#endif
    return err;
}
