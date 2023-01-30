AVFilterGraph *avfilter_graph_alloc(void)
{
    AVFilterGraph *ret = av_mallocz(sizeof(*ret));
    if (!ret)
        return NULL;
    ret->internal = av_mallocz(sizeof(*ret->internal));
    if (!ret->internal) {
        av_freep(&ret);
        return NULL;
    }
    ret->av_class = &filtergraph_class;
    av_opt_set_defaults(ret);
    return ret;
}
