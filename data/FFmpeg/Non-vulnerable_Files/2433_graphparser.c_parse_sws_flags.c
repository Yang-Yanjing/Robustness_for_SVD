static int parse_sws_flags(const char **buf, AVFilterGraph *graph)
{
    char *p = strchr(*buf, ';');
    if (strncmp(*buf, "sws_flags=", 10))
        return 0;
    if (!p) {
        av_log(graph, AV_LOG_ERROR, "sws_flags not terminated with ';'.\n");
        return AVERROR(EINVAL);
    }
    *buf += 4;  
    av_freep(&graph->scale_sws_opts);
    if (!(graph->scale_sws_opts = av_mallocz(p - *buf + 1)))
        return AVERROR(ENOMEM);
    av_strlcpy(graph->scale_sws_opts, *buf, p - *buf + 1);
    *buf = p + 1;
    return 0;
}
