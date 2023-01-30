int avfilter_graph_parse(AVFilterGraph *graph, const char *filters,
                         AVFilterInOut *open_inputs,
                         AVFilterInOut *open_outputs, void *log_ctx)
{
    int ret;
    AVFilterInOut *cur, *match, *inputs = NULL, *outputs = NULL;
    if ((ret = avfilter_graph_parse2(graph, filters, &inputs, &outputs)) < 0)
        goto fail;
    
    if (inputs && !inputs->name)
        inputs->name = av_strdup("in");
    for (cur = inputs; cur; cur = cur->next) {
        if (!cur->name) {
              av_log(log_ctx, AV_LOG_ERROR,
                     "Not enough inputs specified for the \"%s\" filter.\n",
                     cur->filter_ctx->filter->name);
              ret = AVERROR(EINVAL);
              goto fail;
        }
        if (!(match = extract_inout(cur->name, &open_outputs)))
            continue;
        ret = avfilter_link(match->filter_ctx, match->pad_idx,
                            cur->filter_ctx,   cur->pad_idx);
        avfilter_inout_free(&match);
        if (ret < 0)
            goto fail;
    }
    
    if (outputs && !outputs->name)
        outputs->name = av_strdup("out");
    for (cur = outputs; cur; cur = cur->next) {
        if (!cur->name) {
            av_log(log_ctx, AV_LOG_ERROR,
                   "Invalid filterchain containing an unlabelled output pad: \"%s\"\n",
                   filters);
            ret = AVERROR(EINVAL);
            goto fail;
        }
        if (!(match = extract_inout(cur->name, &open_inputs)))
            continue;
        ret = avfilter_link(cur->filter_ctx,   cur->pad_idx,
                            match->filter_ctx, match->pad_idx);
        avfilter_inout_free(&match);
        if (ret < 0)
            goto fail;
    }
 fail:
    if (ret < 0) {
        while (graph->nb_filters)
            avfilter_free(graph->filters[0]);
        av_freep(&graph->filters);
    }
    avfilter_inout_free(&inputs);
    avfilter_inout_free(&outputs);
    avfilter_inout_free(&open_inputs);
    avfilter_inout_free(&open_outputs);
    return ret;
#else
int avfilter_graph_parse(AVFilterGraph *graph, const char *filters,
                         AVFilterInOut **inputs, AVFilterInOut **outputs,
                         void *log_ctx)
{
    return avfilter_graph_parse_ptr(graph, filters, inputs, outputs, log_ctx);
#endif
}
int avfilter_graph_parse_ptr(AVFilterGraph *graph, const char *filters,
                         AVFilterInOut **open_inputs_ptr, AVFilterInOut **open_outputs_ptr,
                         void *log_ctx)
{
    int index = 0, ret = 0;
    char chr = 0;
    AVFilterInOut *curr_inputs = NULL;
    AVFilterInOut *open_inputs  = open_inputs_ptr  ? *open_inputs_ptr  : NULL;
    AVFilterInOut *open_outputs = open_outputs_ptr ? *open_outputs_ptr : NULL;
    if ((ret = parse_sws_flags(&filters, graph)) < 0)
        goto end;
    do {
        AVFilterContext *filter;
        const char *filterchain = filters;
        filters += strspn(filters, WHITESPACES);
        if ((ret = parse_inputs(&filters, &curr_inputs, &open_outputs, log_ctx)) < 0)
            goto end;
        if ((ret = parse_filter(&filter, &filters, graph, index, log_ctx)) < 0)
            goto end;
        if (filter->nb_inputs == 1 && !curr_inputs && !index) {
            
            const char *tmp = "[in]";
            if ((ret = parse_inputs(&tmp, &curr_inputs, &open_outputs, log_ctx)) < 0)
                goto end;
        }
        if ((ret = link_filter_inouts(filter, &curr_inputs, &open_inputs, log_ctx)) < 0)
            goto end;
        if ((ret = parse_outputs(&filters, &curr_inputs, &open_inputs, &open_outputs,
                                 log_ctx)) < 0)
            goto end;
        filters += strspn(filters, WHITESPACES);
        chr = *filters++;
        if (chr == ';' && curr_inputs) {
            av_log(log_ctx, AV_LOG_ERROR,
                   "Invalid filterchain containing an unlabelled output pad: \"%s\"\n",
                   filterchain);
            ret = AVERROR(EINVAL);
            goto end;
        }
        index++;
    } while (chr == ',' || chr == ';');
    if (chr) {
        av_log(log_ctx, AV_LOG_ERROR,
               "Unable to parse graph description substring: \"%s\"\n",
               filters - 1);
        ret = AVERROR(EINVAL);
        goto end;
    }
    if (curr_inputs) {
        
        const char *tmp = "[out]";
        if ((ret = parse_outputs(&tmp, &curr_inputs, &open_inputs, &open_outputs,
                                 log_ctx)) < 0)
            goto end;
    }
end:
    
    if (open_inputs_ptr) *open_inputs_ptr = open_inputs;
    else avfilter_inout_free(&open_inputs);
    if (open_outputs_ptr) *open_outputs_ptr = open_outputs;
    else avfilter_inout_free(&open_outputs);
    avfilter_inout_free(&curr_inputs);
    if (ret < 0) {
        while (graph->nb_filters)
            avfilter_free(graph->filters[0]);
        av_freep(&graph->filters);
    }
    return ret;
}
#else
int avfilter_graph_parse(AVFilterGraph *graph, const char *filters,
                         AVFilterInOut **inputs, AVFilterInOut **outputs,
                         void *log_ctx)
{
    return avfilter_graph_parse_ptr(graph, filters, inputs, outputs, log_ctx);
#endif
}
int avfilter_graph_parse_ptr(AVFilterGraph *graph, const char *filters,
                         AVFilterInOut **open_inputs_ptr, AVFilterInOut **open_outputs_ptr,
                         void *log_ctx)
{
    int index = 0, ret = 0;
    char chr = 0;
    AVFilterInOut *curr_inputs = NULL;
    AVFilterInOut *open_inputs  = open_inputs_ptr  ? *open_inputs_ptr  : NULL;
    AVFilterInOut *open_outputs = open_outputs_ptr ? *open_outputs_ptr : NULL;
    if ((ret = parse_sws_flags(&filters, graph)) < 0)
        goto end;
    do {
        AVFilterContext *filter;
        const char *filterchain = filters;
        filters += strspn(filters, WHITESPACES);
        if ((ret = parse_inputs(&filters, &curr_inputs, &open_outputs, log_ctx)) < 0)
            goto end;
        if ((ret = parse_filter(&filter, &filters, graph, index, log_ctx)) < 0)
            goto end;
        if (filter->nb_inputs == 1 && !curr_inputs && !index) {
            
            const char *tmp = "[in]";
            if ((ret = parse_inputs(&tmp, &curr_inputs, &open_outputs, log_ctx)) < 0)
                goto end;
        }
        if ((ret = link_filter_inouts(filter, &curr_inputs, &open_inputs, log_ctx)) < 0)
            goto end;
        if ((ret = parse_outputs(&filters, &curr_inputs, &open_inputs, &open_outputs,
                                 log_ctx)) < 0)
            goto end;
        filters += strspn(filters, WHITESPACES);
        chr = *filters++;
        if (chr == ';' && curr_inputs) {
            av_log(log_ctx, AV_LOG_ERROR,
                   "Invalid filterchain containing an unlabelled output pad: \"%s\"\n",
                   filterchain);
            ret = AVERROR(EINVAL);
            goto end;
        }
        index++;
    } while (chr == ',' || chr == ';');
    if (chr) {
        av_log(log_ctx, AV_LOG_ERROR,
               "Unable to parse graph description substring: \"%s\"\n",
               filters - 1);
        ret = AVERROR(EINVAL);
        goto end;
    }
    if (curr_inputs) {
        
        const char *tmp = "[out]";
        if ((ret = parse_outputs(&tmp, &curr_inputs, &open_inputs, &open_outputs,
                                 log_ctx)) < 0)
            goto end;
    }
end:
    
    if (open_inputs_ptr) *open_inputs_ptr = open_inputs;
    else avfilter_inout_free(&open_inputs);
    if (open_outputs_ptr) *open_outputs_ptr = open_outputs;
    else avfilter_inout_free(&open_outputs);
    avfilter_inout_free(&curr_inputs);
    if (ret < 0) {
        while (graph->nb_filters)
            avfilter_free(graph->filters[0]);
        av_freep(&graph->filters);
    }
    return ret;
}
