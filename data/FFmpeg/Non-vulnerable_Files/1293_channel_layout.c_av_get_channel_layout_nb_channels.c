int av_get_channel_layout_nb_channels(uint64_t channel_layout)
{
    return av_popcount64(channel_layout);
}
