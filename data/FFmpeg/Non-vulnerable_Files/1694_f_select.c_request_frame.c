static int request_frame(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    SelectContext *select = ctx->priv;
    AVFilterLink *inlink = outlink->src->inputs[0];
    int out_no = FF_OUTLINK_IDX(outlink);
    do {
        int ret = ff_request_frame(inlink);
        if (ret < 0)
            return ret;
    } while (select->select_out != out_no);
    return 0;
}
