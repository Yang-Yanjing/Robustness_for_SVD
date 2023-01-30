static int command(AVFilterContext *ctx, const char *cmd, const char *arg, char *res, int res_len, int flags)
{
    DrawTextContext *s = ctx->priv;
    if (!strcmp(cmd, "reinit")) {
        int ret;
        uninit(ctx);
        s->reinit = 1;
        if ((ret = av_set_options_string(ctx, arg, "=", ":")) < 0)
            return ret;
        if ((ret = init(ctx)) < 0)
            return ret;
        return config_input(ctx->inputs[0]);
    }
    return AVERROR(ENOSYS);
}
