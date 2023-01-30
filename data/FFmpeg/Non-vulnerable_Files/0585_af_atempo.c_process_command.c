static int process_command(AVFilterContext *ctx,
                           const char *cmd,
                           const char *arg,
                           char *res,
                           int res_len,
                           int flags)
{
    return !strcmp(cmd, "tempo") ? yae_set_tempo(ctx, arg) : AVERROR(ENOSYS);
}
