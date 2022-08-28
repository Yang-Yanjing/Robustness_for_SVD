int avfilter_init_dict(AVFilterContext *ctx, AVDictionary **options)
{
    int ret = 0;
    ret = av_opt_set_dict(ctx, options);
    if (ret < 0) {
        av_log(ctx, AV_LOG_ERROR, "Error applying generic filter options.\n");
        return ret;
    }
    if (ctx->filter->flags & AVFILTER_FLAG_SLICE_THREADS &&
        ctx->thread_type & ctx->graph->thread_type & AVFILTER_THREAD_SLICE &&
        ctx->graph->internal->thread_execute) {
        ctx->thread_type       = AVFILTER_THREAD_SLICE;
        ctx->internal->execute = ctx->graph->internal->thread_execute;
    } else {
        ctx->thread_type = 0;
    }
    if (ctx->filter->priv_class) {
        ret = av_opt_set_dict(ctx->priv, options);
        if (ret < 0) {
            av_log(ctx, AV_LOG_ERROR, "Error applying options to the filter.\n");
            return ret;
        }
    }
    if (ctx->filter->init_opaque)
        ret = ctx->filter->init_opaque(ctx, NULL);
    else if (ctx->filter->init)
        ret = ctx->filter->init(ctx);
    else if (ctx->filter->init_dict)
        ret = ctx->filter->init_dict(ctx, options);
    return ret;
}
