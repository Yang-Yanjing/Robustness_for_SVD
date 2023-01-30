char *avfilter_graph_dump(AVFilterGraph *graph, const char *options)
{
    AVBPrint buf;
    char *dump;
    av_bprint_init(&buf, 0, 0);
    avfilter_graph_dump_to_buf(&buf, graph);
    av_bprint_init(&buf, buf.len + 1, buf.len + 1);
    avfilter_graph_dump_to_buf(&buf, graph);
    av_bprint_finalize(&buf, &dump);
    return dump;
}
