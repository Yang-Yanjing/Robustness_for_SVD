static int process_frame(AVFilterLink *inlink, AVFrame *buf)
{
    AVFilterContext *ctx  = inlink->dst;
    ConcatContext *cat    = ctx->priv;
    unsigned in_no = FF_INLINK_IDX(inlink);
    if (in_no < cat->cur_idx) {
        av_log(ctx, AV_LOG_ERROR, "Frame after EOF on input %s\n",
               ctx->input_pads[in_no].name);
        av_frame_free(&buf);
    } else if (in_no >= cat->cur_idx + ctx->nb_outputs) {
        ff_bufqueue_add(ctx, &cat->in[in_no].queue, buf);
    } else {
        return push_frame(ctx, in_no, buf);
    }
    return 0;
}
