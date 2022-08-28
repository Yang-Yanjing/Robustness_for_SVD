FF_ENABLE_DEPRECATION_WARNINGS
int av_buffersrc_buffer(AVFilterContext *ctx, AVFilterBufferRef *buf)
{
    return av_buffersrc_add_ref(ctx, buf, 0);
}
