void avfilter_graph_set_auto_convert(AVFilterGraph *graph, unsigned flags)
{
    graph->disable_auto_convert = flags;
}
