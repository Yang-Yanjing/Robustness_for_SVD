static int process_command(AVFilterContext *ctx, const char *cmd, const char *args,
                           char *res, int res_len, int flags)
{
    ScaleContext *scale = ctx->priv;
    int ret;
    if (   !strcmp(cmd, "width")  || !strcmp(cmd, "w")
        || !strcmp(cmd, "height") || !strcmp(cmd, "h")) {
        int old_w = scale->w;
        int old_h = scale->h;
        AVFilterLink *outlink = ctx->outputs[0];
        av_opt_set(scale, cmd, args, 0);
        if ((ret = config_props(outlink)) < 0) {
            scale->w = old_w;
            scale->h = old_h;
        }
    } else
        ret = AVERROR(ENOSYS);
    return ret;
}
