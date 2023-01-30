int main(int argc, char **argv)
{
    AVFilter *filter;
    AVFilterContext *filter_ctx;
    AVFilterGraph *graph_ctx;
    const char *filter_name;
    const char *filter_args = NULL;
    int i;
    int ret = 0;
    av_log_set_level(AV_LOG_DEBUG);
    if (argc < 2) {
        fprintf(stderr, "Missing filter name as argument\n");
        return 1;
    }
    filter_name = argv[1];
    if (argc > 2)
        filter_args = argv[2];
    
    graph_ctx = avfilter_graph_alloc();
    if (!graph_ctx)
        return 1;
    avfilter_register_all();
    
    if (!(filter = avfilter_get_by_name(filter_name))) {
        fprintf(stderr, "Unrecognized filter with name '%s'\n", filter_name);
        return 1;
    }
    
    if (!(filter_ctx = avfilter_graph_alloc_filter(graph_ctx, filter, filter_name))) {
        fprintf(stderr, "Impossible to open filter with name '%s'\n",
                filter_name);
        return 1;
    }
    if (avfilter_init_str(filter_ctx, filter_args) < 0) {
        fprintf(stderr, "Impossible to init filter '%s' with arguments '%s'\n",
                filter_name, filter_args);
        return 1;
    }
    
    for (i = 0; i < filter_ctx->nb_inputs; i++) {
        AVFilterLink *link = av_mallocz(sizeof(AVFilterLink));
        if (!link) {
            fprintf(stderr, "Unable to allocate memory for filter input link\n");
            ret = 1;
            goto fail;
        }
        link->type = avfilter_pad_get_type(filter_ctx->input_pads, i);
        filter_ctx->inputs[i] = link;
    }
    for (i = 0; i < filter_ctx->nb_outputs; i++) {
        AVFilterLink *link = av_mallocz(sizeof(AVFilterLink));
        if (!link) {
            fprintf(stderr, "Unable to allocate memory for filter output link\n");
            ret = 1;
            goto fail;
        }
        link->type = avfilter_pad_get_type(filter_ctx->output_pads, i);
        filter_ctx->outputs[i] = link;
    }
    if (filter->query_formats)
        filter->query_formats(filter_ctx);
    else
        ff_default_query_formats(filter_ctx);
    print_formats(filter_ctx);
fail:
    avfilter_free(filter_ctx);
    avfilter_graph_free(&graph_ctx);
    fflush(stdout);
    return ret;
}
