void av_buffersink_set_frame_size(AVFilterContext *ctx, unsigned frame_size)
{
    AVFilterLink *inlink = ctx->inputs[0];
    inlink->min_samples = inlink->max_samples =
    inlink->partial_buf_size = frame_size;
}
