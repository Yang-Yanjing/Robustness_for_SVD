void av_bprint_channel_layout(struct AVBPrint *bp,
                              int nb_channels, uint64_t channel_layout)
{
    int i;
    if (nb_channels <= 0)
        nb_channels = av_get_channel_layout_nb_channels(channel_layout);
    for (i = 0; i < FF_ARRAY_ELEMS(channel_layout_map); i++)
        if (nb_channels    == channel_layout_map[i].nb_channels &&
            channel_layout == channel_layout_map[i].layout) {
            av_bprintf(bp, "%s", channel_layout_map[i].name);
            return;
        }
    av_bprintf(bp, "%d channels", nb_channels);
    if (channel_layout) {
        int i, ch;
        av_bprintf(bp, " (");
        for (i = 0, ch = 0; i < 64; i++) {
            if ((channel_layout & (UINT64_C(1) << i))) {
                const char *name = get_channel_name(i);
                if (name) {
                    if (ch > 0)
                        av_bprintf(bp, "+");
                    av_bprintf(bp, "%s", name);
                }
                ch++;
            }
        }
        av_bprintf(bp, ")");
    }
}
