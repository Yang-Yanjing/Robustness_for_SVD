static int request_frame(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    AMergeContext *s = ctx->priv;
    int i, ret;
    for (i = 0; i < s->nb_inputs; i++)
        if (!s->in[i].nb_samples)
            if ((ret = ff_request_frame(ctx->inputs[i])) < 0)
                return ret;
    return 0;
}
