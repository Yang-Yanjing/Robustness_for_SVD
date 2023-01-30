static int config_input(AVFilterLink *inlink)
{
    AVFilterContext   *ctx = inlink->dst;
    FieldOrderContext *s   = ctx->priv;
    return av_image_fill_linesizes(s->line_size, inlink->format, inlink->w);
}
