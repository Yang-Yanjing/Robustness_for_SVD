int64_t av_get_default_channel_layout(int nb_channels) {
    int i;
    for (i = 0; i < FF_ARRAY_ELEMS(channel_layout_map); i++)
        if (nb_channels == channel_layout_map[i].nb_channels)
            return channel_layout_map[i].layout;
    return 0;
}
