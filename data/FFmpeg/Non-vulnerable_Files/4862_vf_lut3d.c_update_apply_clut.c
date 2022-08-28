static AVFrame *update_apply_clut(AVFilterContext *ctx, AVFrame *main,
                                  const AVFrame *second)
{
    AVFilterLink *inlink = ctx->inputs[0];
    update_clut(ctx->priv, second);
    return apply_lut(inlink, main);
}
