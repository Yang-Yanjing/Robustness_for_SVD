FF_DISABLE_DEPRECATION_WARNINGS
int attribute_align_arg av_buffersink_poll_frame(AVFilterContext *ctx)
{
    BufferSinkContext *buf = ctx->priv;
    AVFilterLink *inlink = ctx->inputs[0];
    av_assert0(   !strcmp(ctx->filter->name, "buffersink")
               || !strcmp(ctx->filter->name, "abuffersink")
               || !strcmp(ctx->filter->name, "ffbuffersink")
               || !strcmp(ctx->filter->name, "ffabuffersink"));
    return av_fifo_size(buf->fifo)/FIFO_INIT_ELEMENT_SIZE + ff_poll_frame(inlink);
}
