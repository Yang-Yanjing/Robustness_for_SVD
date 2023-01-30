static int opt_filter_complex(void *optctx, const char *opt, const char *arg)
{
    GROW_ARRAY(filtergraphs, nb_filtergraphs);
    if (!(filtergraphs[nb_filtergraphs - 1] = av_mallocz(sizeof(*filtergraphs[0]))))
        return AVERROR(ENOMEM);
    filtergraphs[nb_filtergraphs - 1]->index      = nb_filtergraphs - 1;
    filtergraphs[nb_filtergraphs - 1]->graph_desc = av_strdup(arg);
    if (!filtergraphs[nb_filtergraphs - 1]->graph_desc)
        return AVERROR(ENOMEM);
    input_stream_potentially_available = 1;
    return 0;
}
