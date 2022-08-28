static int get_channel(char **map, uint64_t *ch, char delim)
{
    char *next = split(*map, delim);
    if (!next && delim == '-')
        return AVERROR(EINVAL);
    *ch = av_get_channel_layout(*map);
    if (av_get_channel_layout_nb_channels(*ch) != 1)
        return AVERROR(EINVAL);
    *map = next;
    return 0;
}
