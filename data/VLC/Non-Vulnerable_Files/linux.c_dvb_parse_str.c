/** Maps a VLC config string to a Linux DVB enum value */
static int dvb_parse_str (const char *str, const dvb_str_map_t *map, size_t n,
                          int def)
{
    if (str != NULL)
    {
        const dvb_str_map_t *p = bsearch (str, map, n, sizeof (*map), scmp);
        if (p != NULL)
            def = p->linux_;
    }
    return def;
}
