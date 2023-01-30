int av_frame_get_buffer(AVFrame *frame, int align)
{
    if (frame->format < 0)
        return AVERROR(EINVAL);
    if (frame->width > 0 && frame->height > 0)
        return get_video_buffer(frame, align);
    else if (frame->nb_samples > 0 && (frame->channel_layout || frame->channels > 0))
        return get_audio_buffer(frame, align);
    return AVERROR(EINVAL);
}
