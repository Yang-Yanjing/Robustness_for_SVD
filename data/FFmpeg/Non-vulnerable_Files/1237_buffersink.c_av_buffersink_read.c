int attribute_align_arg av_buffersink_read(AVFilterContext *ctx, AVFilterBufferRef **buf)
{
    return compat_read(ctx, buf, 0, 0);
}
