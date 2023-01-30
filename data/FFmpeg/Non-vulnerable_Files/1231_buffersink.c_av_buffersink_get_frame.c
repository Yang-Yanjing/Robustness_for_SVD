int attribute_align_arg av_buffersink_get_frame(AVFilterContext *ctx, AVFrame *frame)
{
    return av_buffersink_get_frame_flags(ctx, frame, 0);
}
