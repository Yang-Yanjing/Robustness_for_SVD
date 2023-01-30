} dvb_str_map_t;
static int scmp (const void *a, const void *b)
{
    const char *key = a;
    dvb_str_map_t *entry = b;
    return strcmp (key, entry->vlc);
}
