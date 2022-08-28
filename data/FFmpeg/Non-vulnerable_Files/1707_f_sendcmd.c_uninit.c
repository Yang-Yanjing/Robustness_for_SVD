static av_cold void uninit(AVFilterContext *ctx)
{
    SendCmdContext *sendcmd = ctx->priv;
    int i, j;
    for (i = 0; i < sendcmd->nb_intervals; i++) {
        Interval *interval = &sendcmd->intervals[i];
        for (j = 0; j < interval->nb_commands; j++) {
            Command *cmd = &interval->commands[j];
            av_freep(&cmd->target);
            av_freep(&cmd->command);
            av_freep(&cmd->arg);
        }
        av_freep(&interval->commands);
    }
    av_freep(&sendcmd->intervals);
}
