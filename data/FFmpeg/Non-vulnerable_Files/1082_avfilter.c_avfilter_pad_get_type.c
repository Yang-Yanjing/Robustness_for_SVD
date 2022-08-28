enum AVMediaType avfilter_pad_get_type(const AVFilterPad *pads, int pad_idx)
{
    return pads[pad_idx].type;
}
