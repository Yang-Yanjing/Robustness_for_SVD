AVFilterBufferRef* avfilter_get_audio_buffer_ref_from_arrays(uint8_t **data,
                                                             int linesize,int perms,
                                                             int nb_samples,
                                                             enum AVSampleFormat sample_fmt,
                                                             uint64_t channel_layout)
{
    int channels = av_get_channel_layout_nb_channels(channel_layout);
    return avfilter_get_audio_buffer_ref_from_arrays_channels(data, linesize, perms,
                                                              nb_samples, sample_fmt,
                                                              channels, channel_layout);
}
