const char *av_get_channel_name(uint64_t channel)
{
    int i;
    if (av_get_channel_layout_nb_channels(channel) != 1)
        return NULL;
    for (i = 0; i < 64; i++)
        if ((1ULL<<i) & channel)
            return get_channel_name(i);
    return NULL;
}
