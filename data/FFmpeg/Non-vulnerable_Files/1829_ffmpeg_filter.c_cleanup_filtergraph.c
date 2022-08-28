static void cleanup_filtergraph(FilterGraph *fg)
{
    int i;
    for (i = 0; i < fg->nb_outputs; i++)
        fg->outputs[i]->filter = (AVFilterContext *)NULL;
    for (i = 0; i < fg->nb_inputs; i++)
        fg->inputs[i]->filter = (AVFilterContext *)NULL;
    avfilter_graph_free(&fg->graph);
}
