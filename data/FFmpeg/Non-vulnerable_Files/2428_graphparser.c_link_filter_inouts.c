static int link_filter_inouts(AVFilterContext *filt_ctx,
                              AVFilterInOut **curr_inputs,
                              AVFilterInOut **open_inputs, void *log_ctx)
{
    int pad, ret;
    for (pad = 0; pad < filt_ctx->nb_inputs; pad++) {
        AVFilterInOut *p = *curr_inputs;
        if (p) {
            *curr_inputs = (*curr_inputs)->next;
            p->next = NULL;
        } else if (!(p = av_mallocz(sizeof(*p))))
            return AVERROR(ENOMEM);
        if (p->filter_ctx) {
            ret = link_filter(p->filter_ctx, p->pad_idx, filt_ctx, pad, log_ctx);
            av_freep(&p->name);
            av_freep(&p);
            if (ret < 0)
                return ret;
        } else {
            p->filter_ctx = filt_ctx;
            p->pad_idx = pad;
            append_inout(open_inputs, &p);
        }
    }
    if (*curr_inputs) {
        av_log(log_ctx, AV_LOG_ERROR,
               "Too many inputs specified for the \"%s\" filter.\n",
               filt_ctx->filter->name);
        return AVERROR(EINVAL);
    }
    pad = filt_ctx->nb_outputs;
    while (pad--) {
        AVFilterInOut *currlinkn = av_mallocz(sizeof(AVFilterInOut));
        if (!currlinkn)
            return AVERROR(ENOMEM);
        currlinkn->filter_ctx  = filt_ctx;
        currlinkn->pad_idx = pad;
        insert_inout(curr_inputs, currlinkn);
    }
    return 0;
}
