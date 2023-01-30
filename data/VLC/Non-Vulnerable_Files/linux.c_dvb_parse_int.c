/** Maps a VLC config integer to a Linux DVB enum value */
static int dvb_parse_int (int i, const dvb_int_map_t *map, size_t n, int def)
{
    const void *k = (const void *)(intptr_t)i;
    const dvb_int_map_t *p = bsearch (k, map, n, sizeof (*map), icmp);
    return (p != NULL) ? p->linux_ : def;
}
