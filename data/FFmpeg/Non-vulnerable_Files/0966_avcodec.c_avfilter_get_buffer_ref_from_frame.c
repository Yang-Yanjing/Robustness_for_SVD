AVFilterBufferRef *avfilter_get_buffer_ref_from_frame(enum AVMediaType type,
                                                      const AVFrame *frame,
                                                      int perms)
{
    switch (type) {
    case AVMEDIA_TYPE_VIDEO:
        return avfilter_get_video_buffer_ref_from_frame(frame, perms);
    case AVMEDIA_TYPE_AUDIO:
        return avfilter_get_audio_buffer_ref_from_frame(frame, perms);
    default:
        return NULL;
    }
}
