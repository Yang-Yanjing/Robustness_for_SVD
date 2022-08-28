static av_cold int init(AVFilterContext *ctx)
{
    BlendContext *s = ctx->priv;
    s->tblend = !strcmp(ctx->filter->name, "tblend");
    s->dinput.process = blend_frame;
    return 0;
}
