static int get_channel_idx(char **map, int *ch, char delim, int max_ch)
{
    char *next;
    int len;
    int n = 0;
    if (!*map)
        return AVERROR(EINVAL);
    next = split(*map, delim);
    if (!next && delim == '-')
        return AVERROR(EINVAL);
    len = strlen(*map);
    sscanf(*map, "%d%n", ch, &n);
    if (n != len)
        return AVERROR(EINVAL);
    if (*ch < 0 || *ch > max_ch)
        return AVERROR(EINVAL);
    *map = next;
    return 0;
}
