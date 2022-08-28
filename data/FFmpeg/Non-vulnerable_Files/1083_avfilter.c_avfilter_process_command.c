int avfilter_process_command(AVFilterContext *filter, const char *cmd, const char *arg, char *res, int res_len, int flags)
{
    if(!strcmp(cmd, "ping")){
        char local_res[256] = {0};
        if (!res) {
            res = local_res;
            res_len = sizeof(local_res);
        }
        av_strlcatf(res, res_len, "pong from:%s %s\n", filter->filter->name, filter->name);
        if (res == local_res)
            av_log(filter, AV_LOG_INFO, "%s", res);
        return 0;
    }else if(!strcmp(cmd, "enable")) {
        return set_enable_expr(filter, arg);
    }else if(filter->filter->process_command) {
        return filter->filter->process_command(filter, cmd, arg, res, res_len, flags);
    }
    return AVERROR(ENOSYS);
}
