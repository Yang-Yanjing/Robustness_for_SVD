static int config_input_main(AVFilterLink *inlink)
{
    AlphaMergeContext *merge = inlink->dst->priv;
    merge->is_packed_rgb =
        ff_fill_rgba_map(merge->rgba_map, inlink->format) >= 0;
    return 0;
}
