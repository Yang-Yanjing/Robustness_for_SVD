} dvb_int_map_t;
static int icmp (const void *a, const void *b)
{
    int key = (intptr_t)a;
    const dvb_int_map_t *entry = b;
    return key - entry->vlc;
}
