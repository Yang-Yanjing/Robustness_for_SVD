static int request_frame(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    SidechainCompressContext *s = ctx->priv;
    int i, ret;
    
    for (i = 0; i < 2; i++) {
        AVFilterLink *inlink = ctx->inputs[i];
        if (!s->input_frame[i] &&
            (ret = ff_request_frame(inlink)) < 0)
            return ret;
        
        if (i == 0)
            ctx->inputs[1]->request_samples = s->input_frame[0]->nb_samples;
    }
    return 0;
}
