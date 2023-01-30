static int parse_commands(Command **cmds, int *nb_cmds, int interval_count,
                          const char **buf, void *log_ctx)
{
    int cmd_count = 0;
    int ret, n = 0;
    AVBPrint pbuf;
    *cmds = NULL;
    *nb_cmds = 0;
    while (**buf) {
        Command cmd;
        if ((ret = parse_command(&cmd, cmd_count, interval_count, buf, log_ctx)) < 0)
            return ret;
        cmd_count++;
        
        if (*nb_cmds == n) {
            n = FFMAX(16, 2*n); 
            *cmds = av_realloc_f(*cmds, n, 2*sizeof(Command));
            if (!*cmds) {
                av_log(log_ctx, AV_LOG_ERROR,
                       "Could not (re)allocate command array\n");
                return AVERROR(ENOMEM);
            }
        }
        (*cmds)[(*nb_cmds)++] = cmd;
        *buf += strspn(*buf, SPACES);
        if (**buf && **buf != ';' && **buf != ',') {
            av_log(log_ctx, AV_LOG_ERROR,
                   "Missing separator or extraneous data found at the end of "
                   "interval #%d, in command #%d\n",
                   interval_count, cmd_count);
            av_log(log_ctx, AV_LOG_ERROR,
                   "Command was parsed as: flags:[%s] target:%s command:%s arg:%s\n",
                   make_command_flags_str(&pbuf, cmd.flags), cmd.target, cmd.command, cmd.arg);
            return AVERROR(EINVAL);
        }
        if (**buf == ';')
            break;
        if (**buf == ',')
            (*buf)++;
    }
    return 0;
}
