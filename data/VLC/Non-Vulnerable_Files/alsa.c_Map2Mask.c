};
static int Map2Mask (vlc_object_t *obj, const snd_pcm_chmap_t *restrict map)
{
    uint16_t mask = 0;
    for (unsigned i = 0; i < map->channels; i++)
    {
        const unsigned pos = map->pos[i];
        uint_fast16_t vlc_chan = 0;
        if (pos < sizeof (vlc_chans) / sizeof (vlc_chans[0]))
            vlc_chan = vlc_chans[pos];
        if (vlc_chan == 0)
        {
            msg_Dbg (obj, " %s channel %u position %u", "unsupported", i, pos);
            return -1;
        }
        if (mask & vlc_chan)
        {
            msg_Dbg (obj, " %s channel %u position %u", "duplicate", i, pos);
            return -1;
        }
        mask |= vlc_chan;
    }
    return mask;
}
