static int request_frame(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    AResampleContext *aresample = ctx->priv;
    int ret;
    
    if (aresample->more_data) {
        AVFrame *outsamplesref;
        if (flush_frame(outlink, 0, &outsamplesref) >= 0) {
            return ff_filter_frame(outlink, outsamplesref);
        }
    }
    aresample->more_data = 0;
    
    aresample->req_fullfilled = 0;
    do{
        ret = ff_request_frame(ctx->inputs[0]);
    }while(!aresample->req_fullfilled && ret>=0);
    
    if (ret == AVERROR_EOF) {
        AVFrame *outsamplesref;
        if ((ret = flush_frame(outlink, 1, &outsamplesref)) < 0)
            return ret;
        return ff_filter_frame(outlink, outsamplesref);
    }
    return ret;
}
