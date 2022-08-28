AVFilterBufferRef *avfilter_get_audio_buffer_ref_from_frame(const AVFrame *frame,
                                                            int perms)
{
    AVFilterBufferRef *samplesref;
    int channels = av_frame_get_channels(frame);
    int64_t layout = av_frame_get_channel_layout(frame);
    if (layout && av_get_channel_layout_nb_channels(layout) != av_frame_get_channels(frame)) {
        av_log(NULL, AV_LOG_ERROR, "Layout indicates a different number of channels than actually present\n");
        return NULL;
    }
    samplesref = avfilter_get_audio_buffer_ref_from_arrays_channels(
        (uint8_t **)frame->extended_data, frame->linesize[0], perms,
        frame->nb_samples, frame->format, channels, layout);
    if (!samplesref)
        return NULL;
    if (avfilter_copy_frame_props(samplesref, frame) < 0) {
        samplesref->buf->data[0] = NULL;
        avfilter_unref_bufferp(&samplesref);
    }
    return samplesref;
}
