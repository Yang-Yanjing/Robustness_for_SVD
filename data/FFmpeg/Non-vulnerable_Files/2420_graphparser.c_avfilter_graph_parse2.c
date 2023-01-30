int avfilter_graph_parse2(AVFilterGraph *graph, const char *filters,
                          AVFilterInOut **inputs,
                          AVFilterInOut **outputs)
{
    int index = 0, ret = 0;
    char chr = 0;
    AVFilterInOut *curr_inputs = NULL, *open_inputs = NULL, *open_outputs = NULL;
    filters += strspn(filters, WHITESPACES);
    if ((ret = parse_sws_flags(&filters, graph)) < 0)
        goto fail;
    do {
        AVFilterContext *filter;
        filters += strspn(filters, WHITESPACES);
        if ((ret = parse_inputs(&filters, &curr_inputs, &open_outputs, graph)) < 0)
            goto end;
        if ((ret = parse_filter(&filter, &filters, graph, index, graph)) < 0)
            goto end;
        if ((ret = link_filter_inouts(filter, &curr_inputs, &open_inputs, graph)) < 0)
            goto end;
        if ((ret = parse_outputs(&filters, &curr_inputs, &open_inputs, &open_outputs,
                                 graph)) < 0)
            goto end;
        filters += strspn(filters, WHITESPACES);
        chr = *filters++;
        if (chr == ';' && curr_inputs)
            append_inout(&open_outputs, &curr_inputs);
        index++;
    } while (chr == ',' || chr == ';');
    if (chr) {
        av_log(graph, AV_LOG_ERROR,
               "Unable to parse graph description substring: \"%s\"\n",
               filters - 1);
        ret = AVERROR(EINVAL);
        goto end;
    }
    append_inout(&open_outputs, &curr_inputs);
    *inputs  = open_inputs;
    *outputs = open_outputs;
    return 0;
 fail:end:
    while (graph->nb_filters)
        avfilter_free(graph->filters[0]);
    av_freep(&graph->filters);
    avfilter_inout_free(&open_inputs);
    avfilter_inout_free(&open_outputs);
    avfilter_inout_free(&curr_inputs);
    *inputs  = NULL;
    *outputs = NULL;
    return ret;
}
