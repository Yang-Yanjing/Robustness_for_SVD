const char *avfilter_pad_get_name(const AVFilterPad *pads, int pad_idx)
{
    return pads[pad_idx].name;
}
