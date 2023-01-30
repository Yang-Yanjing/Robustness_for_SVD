static AVFrame *get_video_buffer(AVFilterLink *inlink, int w, int h)
{
    AVFilterContext *ctx = inlink->dst;
    unsigned in_no = FF_INLINK_IDX(inlink);
    AVFilterLink *outlink = ctx->outputs[in_no % ctx->nb_outputs];
    return ff_get_video_buffer(outlink, w, h);
}
