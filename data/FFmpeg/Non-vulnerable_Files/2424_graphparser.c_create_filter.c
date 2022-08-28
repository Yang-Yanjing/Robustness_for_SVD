static int create_filter(AVFilterContext **filt_ctx, AVFilterGraph *ctx, int index,
                         const char *filt_name, const char *args, void *log_ctx)
{
    AVFilter *filt;
    char inst_name[30];
    char *tmp_args = NULL;
    int ret;
    snprintf(inst_name, sizeof(inst_name), "Parsed_%s_%d", filt_name, index);
    filt = avfilter_get_by_name(filt_name);
    if (!filt) {
        av_log(log_ctx, AV_LOG_ERROR,
               "No such filter: '%s'\n", filt_name);
        return AVERROR(EINVAL);
    }
    *filt_ctx = avfilter_graph_alloc_filter(ctx, filt, inst_name);
    if (!*filt_ctx) {
        av_log(log_ctx, AV_LOG_ERROR,
               "Error creating filter '%s'\n", filt_name);
        return AVERROR(ENOMEM);
    }
    if (!strcmp(filt_name, "scale") && (!args || !strstr(args, "flags")) &&
        ctx->scale_sws_opts) {
        if (args) {
            tmp_args = av_asprintf("%s:%s",
                    args, ctx->scale_sws_opts);
            if (!tmp_args)
                return AVERROR(ENOMEM);
            args = tmp_args;
        } else
            args = ctx->scale_sws_opts;
    }
    ret = avfilter_init_str(*filt_ctx, args);
    if (ret < 0) {
        av_log(log_ctx, AV_LOG_ERROR,
               "Error initializing filter '%s'", filt_name);
        if (args)
            av_log(log_ctx, AV_LOG_ERROR, " with args '%s'", args);
        av_log(log_ctx, AV_LOG_ERROR, "\n");
        avfilter_free(*filt_ctx);
        *filt_ctx = NULL;
    }
    av_free(tmp_args);
    return ret;
}
