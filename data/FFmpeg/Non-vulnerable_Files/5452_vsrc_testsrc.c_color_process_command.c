static int color_process_command(AVFilterContext *ctx, const char *cmd, const char *args,
                                 char *res, int res_len, int flags)
{
    TestSourceContext *test = ctx->priv;
    int ret;
    if (!strcmp(cmd, "color") || !strcmp(cmd, "c")) {
        uint8_t color_rgba[4];
        ret = av_parse_color(color_rgba, args, -1, ctx);
        if (ret < 0)
            return ret;
        memcpy(test->color_rgba, color_rgba, sizeof(color_rgba));
        ff_draw_color(&test->draw, &test->color, test->color_rgba);
        test->draw_once_reset = 1;
        return 0;
    }
    return AVERROR(ENOSYS);
}
