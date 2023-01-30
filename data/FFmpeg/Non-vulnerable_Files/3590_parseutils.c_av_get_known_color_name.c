const char *av_get_known_color_name(int color_idx, const uint8_t **rgbp)
{
    const ColorEntry *color;
    if ((unsigned)color_idx >= FF_ARRAY_ELEMS(color_table))
        return NULL;
    color = &color_table[color_idx];
    if (rgbp)
        *rgbp = color->rgb_color;
    return color->name;
}
