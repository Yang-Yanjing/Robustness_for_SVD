 */
static unsigned SetupChannelsFixed(const snd_pcm_chmap_t *restrict map,
                               uint16_t *restrict maskp, uint8_t *restrict tab)
{
    uint32_t chans_out[AOUT_CHAN_MAX];
    uint16_t mask = 0;
    for (unsigned i = 0; i < map->channels; i++)
    {
        uint_fast16_t vlc_chan = vlc_chans[map->pos[i]];
        chans_out[i] = vlc_chan;
        mask |= vlc_chan;
    }
    *maskp = mask;
    return aout_CheckChannelReorder(NULL, chans_out, mask, tab);
}
