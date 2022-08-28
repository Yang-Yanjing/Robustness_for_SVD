AVFilterChannelLayouts *avfilter_make_format64_list(const int64_t *fmts)
{
    MAKE_FORMAT_LIST(AVFilterChannelLayouts,
                     channel_layouts, nb_channel_layouts);
    if (count)
        memcpy(formats->channel_layouts, fmts,
               sizeof(*formats->channel_layouts) * count);
    return formats;
}
