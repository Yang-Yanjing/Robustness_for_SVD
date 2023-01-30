int ff_add_channel_layout(AVFilterChannelLayouts **l, uint64_t channel_layout)
{
    av_assert1(!(*l && (*l)->all_layouts));
    ADD_FORMAT(l, channel_layout, uint64_t, channel_layouts, nb_channel_layouts);
    return 0;
}
