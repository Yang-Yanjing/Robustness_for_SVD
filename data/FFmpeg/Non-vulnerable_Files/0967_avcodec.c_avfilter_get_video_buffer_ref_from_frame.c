FF_DISABLE_DEPRECATION_WARNINGS
AVFilterBufferRef *avfilter_get_video_buffer_ref_from_frame(const AVFrame *frame,
                                                            int perms)
{
    AVFilterBufferRef *picref =
        avfilter_get_video_buffer_ref_from_arrays(frame->data, frame->linesize, perms,
                                                  frame->width, frame->height,
                                                  frame->format);
    if (!picref)
        return NULL;
    if (avfilter_copy_frame_props(picref, frame) < 0) {
        picref->buf->data[0] = NULL;
        avfilter_unref_bufferp(&picref);
    }
    return picref;
}
