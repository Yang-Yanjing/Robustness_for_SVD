int init_complex_filtergraph(FilterGraph *fg)
{
    AVFilterInOut *inputs, *outputs, *cur;
    AVFilterGraph *graph;
    int ret = 0;
    

    graph = avfilter_graph_alloc();
    if (!graph)
        return AVERROR(ENOMEM);
    ret = avfilter_graph_parse2(graph, fg->graph_desc, &inputs, &outputs);
    if (ret < 0)
        goto fail;
    for (cur = inputs; cur; cur = cur->next)
        init_input_filter(fg, cur);
    for (cur = outputs; cur;) {
        GROW_ARRAY(fg->outputs, fg->nb_outputs);
        fg->outputs[fg->nb_outputs - 1] = av_mallocz(sizeof(*fg->outputs[0]));
        if (!fg->outputs[fg->nb_outputs - 1])
            exit_program(1);
        fg->outputs[fg->nb_outputs - 1]->graph   = fg;
        fg->outputs[fg->nb_outputs - 1]->out_tmp = cur;
        fg->outputs[fg->nb_outputs - 1]->type    = avfilter_pad_get_type(cur->filter_ctx->output_pads,
                                                                         cur->pad_idx);
        fg->outputs[fg->nb_outputs - 1]->name = describe_filter_link(fg, cur, 0);
        cur = cur->next;
        fg->outputs[fg->nb_outputs - 1]->out_tmp->next = NULL;
    }
fail:
    avfilter_inout_free(&inputs);
    avfilter_graph_free(&graph);
    return ret;
}
