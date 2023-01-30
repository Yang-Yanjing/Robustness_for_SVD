AVFrame *ff_default_get_audio_buffer(AVFilterLink *link, int nb_samples)
{
    AVFrame *frame = av_frame_alloc();
    int channels = link->channels;
    int ret;
    av_assert0(channels == av_get_channel_layout_nb_channels(link->channel_layout) || !av_get_channel_layout_nb_channels(link->channel_layout));
    if (!frame)
        return NULL;
    frame->nb_samples     = nb_samples;
    frame->format         = link->format;
    av_frame_set_channels(frame, link->channels);
    frame->channel_layout = link->channel_layout;
    frame->sample_rate    = link->sample_rate;
    ret = av_frame_get_buffer(frame, 0);
    if (ret < 0) {
        av_frame_free(&frame);
        return NULL;
    }
    av_samples_set_silence(frame->extended_data, 0, nb_samples, channels,
                           link->format);
    return frame;
}
