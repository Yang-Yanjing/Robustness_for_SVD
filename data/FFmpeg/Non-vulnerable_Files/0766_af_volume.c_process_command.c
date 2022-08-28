static int process_command(AVFilterContext *ctx, const char *cmd, const char *args,
                           char *res, int res_len, int flags)
{
    VolumeContext *vol = ctx->priv;
    int ret = AVERROR(ENOSYS);
    if (!strcmp(cmd, "volume")) {
        if ((ret = set_expr(&vol->volume_pexpr, args, ctx)) < 0)
            return ret;
        if (vol->eval_mode == EVAL_MODE_ONCE)
            set_volume(ctx);
    }
    return ret;
}
