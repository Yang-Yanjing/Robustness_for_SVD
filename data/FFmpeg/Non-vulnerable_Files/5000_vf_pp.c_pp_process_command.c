static int pp_process_command(AVFilterContext *ctx, const char *cmd, const char *args,
                              char *res, int res_len, int flags)
{
    PPFilterContext *pp = ctx->priv;
    if (!strcmp(cmd, "quality")) {
        pp->mode_id = av_clip(strtol(args, NULL, 10), 0, PP_QUALITY_MAX);
        return 0;
    }
    return AVERROR(ENOSYS);
}
