static int filter_frame(AVFilterLink *inlink, AVFrame *ref)
{
    AVFilterContext *ctx = inlink->dst;
    SendCmdContext *sendcmd = ctx->priv;
    int64_t ts;
    int i, j, ret;
    if (ref->pts == AV_NOPTS_VALUE)
        goto end;
    ts = av_rescale_q(ref->pts, inlink->time_base, AV_TIME_BASE_Q);
#define WITHIN_INTERVAL(ts, start_ts, end_ts) ((ts) >= (start_ts) && (ts) < (end_ts))
    for (i = 0; i < sendcmd->nb_intervals; i++) {
        Interval *interval = &sendcmd->intervals[i];
        int flags = 0;
        if (!interval->enabled && WITHIN_INTERVAL(ts, interval->start_ts, interval->end_ts)) {
            flags += COMMAND_FLAG_ENTER;
            interval->enabled = 1;
        }
        if (interval->enabled && !WITHIN_INTERVAL(ts, interval->start_ts, interval->end_ts)) {
            flags += COMMAND_FLAG_LEAVE;
            interval->enabled = 0;
        }
        if (flags) {
            AVBPrint pbuf;
            av_log(ctx, AV_LOG_VERBOSE,
                   "[%s] interval #%d start_ts:%f end_ts:%f ts:%f\n",
                   make_command_flags_str(&pbuf, flags), interval->index,
                   (double)interval->start_ts/1000000, (double)interval->end_ts/1000000,
                   (double)ts/1000000);
            for (j = 0; flags && j < interval->nb_commands; j++) {
                Command *cmd = &interval->commands[j];
                char buf[1024];
                if (cmd->flags & flags) {
                    av_log(ctx, AV_LOG_VERBOSE,
                           "Processing command #%d target:%s command:%s arg:%s\n",
                           cmd->index, cmd->target, cmd->command, cmd->arg);
                    ret = avfilter_graph_send_command(inlink->graph,
                                                      cmd->target, cmd->command, cmd->arg,
                                                      buf, sizeof(buf),
                                                      AVFILTER_CMD_FLAG_ONE);
                    av_log(ctx, AV_LOG_VERBOSE,
                           "Command reply for command #%d: ret:%s res:%s\n",
                           cmd->index, av_err2str(ret), buf);
                }
            }
        }
    }
end:
    switch (inlink->type) {
    case AVMEDIA_TYPE_VIDEO:
    case AVMEDIA_TYPE_AUDIO:
        return ff_filter_frame(inlink->dst->outputs[0], ref);
    }
    return AVERROR(ENOSYS);
}
