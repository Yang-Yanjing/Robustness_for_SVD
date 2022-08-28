int ifilter_parameters_from_frame(InputFilter *ifilter, const AVFrame *frame)
{
    av_buffer_unref(&ifilter->hw_frames_ctx);
    ifilter->format = frame->format;
    ifilter->width               = frame->width;
    ifilter->height              = frame->height;
    ifilter->sample_aspect_ratio = frame->sample_aspect_ratio;
    ifilter->sample_rate         = frame->sample_rate;
    ifilter->channels            = av_frame_get_channels(frame);
    ifilter->channel_layout      = frame->channel_layout;
    if (frame->hw_frames_ctx) {
        ifilter->hw_frames_ctx = av_buffer_ref(frame->hw_frames_ctx);
        if (!ifilter->hw_frames_ctx)
            return AVERROR(ENOMEM);
    }
    return 0;
}
