const char *av_get_channel_description(uint64_t channel)
{
    int i;
    if (av_get_channel_layout_nb_channels(channel) != 1)
        return NULL;
    for (i = 0; i < FF_ARRAY_ELEMS(channel_names); i++)
        if ((1ULL<<i) & channel)
            return channel_names[i].description;
    return NULL;
}
