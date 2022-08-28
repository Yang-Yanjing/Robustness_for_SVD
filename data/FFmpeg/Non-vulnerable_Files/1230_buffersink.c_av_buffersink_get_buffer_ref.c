int attribute_align_arg av_buffersink_get_buffer_ref(AVFilterContext *ctx,
                                                     AVFilterBufferRef **bufref, int flags)
{
    *bufref = NULL;
    av_assert0(    !strcmp(ctx->filter->name, "buffersink")
                || !strcmp(ctx->filter->name, "abuffersink")
                || !strcmp(ctx->filter->name, "ffbuffersink")
                || !strcmp(ctx->filter->name, "ffabuffersink"));
    return compat_read(ctx, bufref, 0, flags);
}
