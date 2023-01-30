static int request_inlink(AVFilterContext *ctx, int lid)
{
    int ret = 0;
    FieldMatchContext *fm = ctx->priv;
    if (!fm->got_frame[lid]) {
        AVFilterLink *inlink = ctx->inputs[lid];
        ret = ff_request_frame(inlink);
        if (ret == AVERROR_EOF) { 
            fm->eof |= 1 << lid;
            ret = filter_frame(inlink, NULL);
        }
    }
    return ret;
}
