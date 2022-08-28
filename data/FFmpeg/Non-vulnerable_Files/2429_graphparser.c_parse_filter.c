static int parse_filter(AVFilterContext **filt_ctx, const char **buf, AVFilterGraph *graph,
                        int index, void *log_ctx)
{
    char *opts = NULL;
    char *name = av_get_token(buf, "=,;[\n");
    int ret;
    if (**buf == '=') {
        (*buf)++;
        opts = av_get_token(buf, "[],;\n");
    }
    ret = create_filter(filt_ctx, graph, index, name, opts, log_ctx);
    av_free(name);
    av_free(opts);
    return ret;
}
