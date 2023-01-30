 */
static unsigned SetupChannels (vlc_object_t *obj, snd_pcm_t *pcm,
                               uint16_t *restrict mask, uint8_t *restrict tab)
{
    snd_pcm_chmap_query_t **maps = snd_pcm_query_chmaps (pcm);
    if (maps == NULL)
    {   /* Fallback to default order if unknown */
        msg_Dbg(obj, "channels map not provided");
        return 0;
    }
    /* Find most appropriate available channels map */
    unsigned best_offset, best_score = 0, to_reorder = 0;
    for (snd_pcm_chmap_query_t *const *p = maps; *p != NULL; p++)
    {
        snd_pcm_chmap_query_t *map = *p;
        switch (map->type)
        {
            case SND_CHMAP_TYPE_FIXED:
            case SND_CHMAP_TYPE_PAIRED:
            case SND_CHMAP_TYPE_VAR:
                break;
            default:
                msg_Err (obj, "unknown channels map type %u", map->type);
                continue;
        }
        int chans = Map2Mask (obj, &map->map);
        if (chans == -1)
            continue;
        unsigned score = (popcount (chans & *mask) << 8)
                       | (255 - popcount (chans));
        if (score > best_score)
        {
            best_offset = p - maps;
            best_score = score;
        }
    }
    if (best_score == 0)
    {
        msg_Err (obj, "cannot find supported channels map");
        goto out;
    }
    const snd_pcm_chmap_t *map = &maps[best_offset]->map;
    msg_Dbg (obj, "using channels map %u, type %u, %u channel(s)", best_offset,
             maps[best_offset]->type, map->channels);
    /* Setup channels map */
    to_reorder = SetupChannelsFixed(map, mask, tab);
    /* TODO: avoid reordering for PAIRED and VAR types */
    //snd_pcm_set_chmap (pcm, ...)
out:
    snd_pcm_free_chmaps (maps);
    return to_reorder;
}
