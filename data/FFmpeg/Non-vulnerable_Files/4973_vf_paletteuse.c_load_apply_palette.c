static AVFrame *load_apply_palette(AVFilterContext *ctx, AVFrame *main,
                                   const AVFrame *second)
{
    AVFilterLink *inlink = ctx->inputs[0];
    PaletteUseContext *s = ctx->priv;
    if (!s->palette_loaded) {
        load_palette(s, second);
    }
    return apply_palette(inlink, main);
}
