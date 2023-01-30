static int process_command(AVFilterContext *ctx, const char *cmd, const char *args,
                           char *res, int res_len, int flags)
{
    SPPContext *s = ctx->priv;
    if (!strcmp(cmd, "level")) {
        if (!strcmp(args, "max"))
            s->log2_count = MAX_LEVEL;
        else
            s->log2_count = av_clip(strtol(args, NULL, 10), 0, MAX_LEVEL);
        return 0;
    }
    return AVERROR(ENOSYS);
}
