void avfilter_free(AVFilterContext *filter)
{
    int i;
    if (!filter)
        return;
    if (filter->graph)
        ff_filter_graph_remove_filter(filter->graph, filter);
    if (filter->filter->uninit)
        filter->filter->uninit(filter);
    for (i = 0; i < filter->nb_inputs; i++) {
        free_link(filter->inputs[i]);
    }
    for (i = 0; i < filter->nb_outputs; i++) {
        free_link(filter->outputs[i]);
    }
    if (filter->filter->priv_class)
        av_opt_free(filter->priv);
    av_freep(&filter->name);
    av_freep(&filter->input_pads);
    av_freep(&filter->output_pads);
    av_freep(&filter->inputs);
    av_freep(&filter->outputs);
    av_freep(&filter->priv);
    while(filter->command_queue){
        ff_command_queue_pop(filter);
    }
    av_opt_free(filter);
    av_expr_free(filter->enable);
    filter->enable = NULL;
    av_freep(&filter->var_values);
    av_freep(&filter->internal);
    av_free(filter);
}
