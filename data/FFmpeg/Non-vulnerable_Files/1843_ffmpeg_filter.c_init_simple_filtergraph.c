                  GET_CH_LAYOUT_NAME)
int init_simple_filtergraph(InputStream *ist, OutputStream *ost)
{
    FilterGraph *fg = av_mallocz(sizeof(*fg));
    if (!fg)
        exit_program(1);
    fg->index = nb_filtergraphs;
    GROW_ARRAY(fg->outputs, fg->nb_outputs);
    if (!(fg->outputs[0] = av_mallocz(sizeof(*fg->outputs[0]))))
        exit_program(1);
    fg->outputs[0]->ost   = ost;
    fg->outputs[0]->graph = fg;
    fg->outputs[0]->format = -1;
    ost->filter = fg->outputs[0];
    GROW_ARRAY(fg->inputs, fg->nb_inputs);
    if (!(fg->inputs[0] = av_mallocz(sizeof(*fg->inputs[0]))))
        exit_program(1);
    fg->inputs[0]->ist   = ist;
    fg->inputs[0]->graph = fg;
    fg->inputs[0]->format = -1;
    fg->inputs[0]->frame_queue = av_fifo_alloc(8 * sizeof(AVFrame*));
    if (!fg->inputs[0]->frame_queue)
        exit_program(1);
    GROW_ARRAY(ist->filters, ist->nb_filters);
    ist->filters[ist->nb_filters - 1] = fg->inputs[0];
    GROW_ARRAY(filtergraphs, nb_filtergraphs);
    filtergraphs[nb_filtergraphs - 1] = fg;
    return 0;
}
