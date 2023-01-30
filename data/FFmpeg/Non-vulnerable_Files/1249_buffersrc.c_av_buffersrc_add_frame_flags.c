                                           AVFrame *frame, int flags);
int attribute_align_arg av_buffersrc_add_frame_flags(AVFilterContext *ctx, AVFrame *frame, int flags)
{
    AVFrame *copy = NULL;
    int ret = 0;
    if (frame && frame->channel_layout &&
        av_get_channel_layout_nb_channels(frame->channel_layout) != av_frame_get_channels(frame)) {
        av_log(ctx, AV_LOG_ERROR, "Layout indicates a different number of channels than actually present\n");
        return AVERROR(EINVAL);
    }
    if (!(flags & AV_BUFFERSRC_FLAG_KEEP_REF) || !frame)
        return av_buffersrc_add_frame_internal(ctx, frame, flags);
    if (!(copy = av_frame_alloc()))
        return AVERROR(ENOMEM);
    ret = av_frame_ref(copy, frame);
    if (ret >= 0)
        ret = av_buffersrc_add_frame_internal(ctx, copy, flags);
    av_frame_free(&copy);
    return ret;
}
