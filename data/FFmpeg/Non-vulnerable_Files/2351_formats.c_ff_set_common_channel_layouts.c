    return 0;
int ff_set_common_channel_layouts(AVFilterContext *ctx,
                                  AVFilterChannelLayouts *layouts)
{
    SET_COMMON_FORMATS(ctx, layouts, in_channel_layouts, out_channel_layouts,
                       ff_channel_layouts_ref, channel_layouts);
}
