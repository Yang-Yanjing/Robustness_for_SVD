static int color_config_props(AVFilterLink *inlink)
{
    AVFilterContext *ctx = inlink->src;
    TestSourceContext *test = ctx->priv;
    int ret;
    ff_draw_init(&test->draw, inlink->format, 0);
    ff_draw_color(&test->draw, &test->color, test->color_rgba);
    test->w = ff_draw_round_to_sub(&test->draw, 0, -1, test->w);
    test->h = ff_draw_round_to_sub(&test->draw, 1, -1, test->h);
    if (av_image_check_size(test->w, test->h, 0, ctx) < 0)
        return AVERROR(EINVAL);
    if ((ret = config_props(inlink)) < 0)
        return ret;
    return 0;
}
