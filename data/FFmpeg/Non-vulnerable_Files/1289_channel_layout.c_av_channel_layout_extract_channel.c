uint64_t av_channel_layout_extract_channel(uint64_t channel_layout, int index)
{
    int i;
    if (av_get_channel_layout_nb_channels(channel_layout) <= index)
        return 0;
    for (i = 0; i < 64; i++) {
        if ((1ULL << i) & channel_layout && !index--)
            return 1ULL << i;
    }
    return 0;
}
