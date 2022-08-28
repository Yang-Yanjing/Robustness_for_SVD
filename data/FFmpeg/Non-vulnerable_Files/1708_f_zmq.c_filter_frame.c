static int filter_frame(AVFilterLink *inlink, AVFrame *ref)
{
    AVFilterContext *ctx = inlink->dst;
    ZMQContext *zmq = ctx->priv;
    while (1) {
        char cmd_buf[1024];
        char *recv_buf, *send_buf;
        int recv_buf_size;
        Command cmd = {0};
        int ret;
        
        if (recv_msg(ctx, &recv_buf, &recv_buf_size) < 0)
            break;
        zmq->command_count++;
        
        if (parse_command(&cmd, recv_buf, ctx) < 0) {
            av_log(ctx, AV_LOG_ERROR, "Could not parse command #%d\n", zmq->command_count);
            goto end;
        }
        
        av_log(ctx, AV_LOG_VERBOSE,
               "Processing command #%d target:%s command:%s arg:%s\n",
               zmq->command_count, cmd.target, cmd.command, cmd.arg);
        ret = avfilter_graph_send_command(inlink->graph,
                                          cmd.target, cmd.command, cmd.arg,
                                          cmd_buf, sizeof(cmd_buf),
                                          AVFILTER_CMD_FLAG_ONE);
        send_buf = av_asprintf("%d %s%s%s",
                               -ret, av_err2str(ret), cmd_buf[0] ? "\n" : "", cmd_buf);
        if (!send_buf) {
            ret = AVERROR(ENOMEM);
            goto end;
        }
        av_log(ctx, AV_LOG_VERBOSE,
               "Sending command reply for command #%d:\n%s\n",
               zmq->command_count, send_buf);
        if (zmq_send(zmq->responder, send_buf, strlen(send_buf), 0) == -1)
            av_log(ctx, AV_LOG_ERROR, "Failed to send reply for command #%d: %s\n",
                   zmq->command_count, zmq_strerror(ret));
    end:
        av_freep(&send_buf);
        av_freep(&recv_buf);
        recv_buf_size = 0;
        av_freep(&cmd.target);
        av_freep(&cmd.command);
        av_freep(&cmd.arg);
    }
    return ff_filter_frame(ctx->outputs[0], ref);
}
