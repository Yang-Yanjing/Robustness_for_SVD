AVFilterContext *ff_filter_alloc(const AVFilter *filter, const char *inst_name)
{
    AVFilterContext *ret;
    if (!filter)
        return NULL;
    ret = av_mallocz(sizeof(AVFilterContext));
    if (!ret)
        return NULL;
    ret->av_class = &avfilter_class;
    ret->filter   = filter;
    ret->name     = inst_name ? av_strdup(inst_name) : NULL;
    if (filter->priv_size) {
        ret->priv     = av_mallocz(filter->priv_size);
        if (!ret->priv)
            goto err;
    }
    av_opt_set_defaults(ret);
    if (filter->priv_class) {
        *(const AVClass**)ret->priv = filter->priv_class;
        av_opt_set_defaults(ret->priv);
    }
    ret->internal = av_mallocz(sizeof(*ret->internal));
    if (!ret->internal)
        goto err;
    ret->internal->execute = default_execute;
    ret->nb_inputs = avfilter_pad_count(filter->inputs);
    if (ret->nb_inputs ) {
        ret->input_pads   = av_malloc_array(ret->nb_inputs, sizeof(AVFilterPad));
        if (!ret->input_pads)
            goto err;
        memcpy(ret->input_pads, filter->inputs, sizeof(AVFilterPad) * ret->nb_inputs);
        ret->inputs       = av_mallocz_array(ret->nb_inputs, sizeof(AVFilterLink*));
        if (!ret->inputs)
            goto err;
    }
    ret->nb_outputs = avfilter_pad_count(filter->outputs);
    if (ret->nb_outputs) {
        ret->output_pads  = av_malloc_array(ret->nb_outputs, sizeof(AVFilterPad));
        if (!ret->output_pads)
            goto err;
        memcpy(ret->output_pads, filter->outputs, sizeof(AVFilterPad) * ret->nb_outputs);
        ret->outputs      = av_mallocz_array(ret->nb_outputs, sizeof(AVFilterLink*));
        if (!ret->outputs)
            goto err;
    }
#if FF_API_FOO_COUNT
FF_DISABLE_DEPRECATION_WARNINGS
    ret->output_count = ret->nb_outputs;
    ret->input_count  = ret->nb_inputs;
FF_ENABLE_DEPRECATION_WARNINGS
#endif
    return ret;
err:
    av_freep(&ret->inputs);
    av_freep(&ret->input_pads);
    ret->nb_inputs = 0;
    av_freep(&ret->outputs);
    av_freep(&ret->output_pads);
    ret->nb_outputs = 0;
    av_freep(&ret->priv);
    av_freep(&ret->internal);
    av_free(ret);
    return NULL;
}
