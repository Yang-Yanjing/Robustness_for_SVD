AVFILTER_DEFINE_CLASS(color);
static void color_fill_picture(AVFilterContext *ctx, AVFrame *picref)
{
    TestSourceContext *test = ctx->priv;
    ff_fill_rectangle(&test->draw, &test->color,
                      picref->data, picref->linesize,
                      0, 0, test->w, test->h);
}
