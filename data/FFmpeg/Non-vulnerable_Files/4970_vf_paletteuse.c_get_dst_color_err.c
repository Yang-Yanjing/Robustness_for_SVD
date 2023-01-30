static av_always_inline int get_dst_color_err(struct cache_node *cache,
                                              uint32_t c, const struct color_node *map,
                                              const uint32_t *palette,
                                              int *er, int *eg, int *eb,
                                              const enum color_search_method search_method)
{
    const uint8_t r = c >> 16 & 0xff;
    const uint8_t g = c >>  8 & 0xff;
    const uint8_t b = c       & 0xff;
    const int dstx = color_get(cache, c, r, g, b, map, palette, search_method);
    const uint32_t dstc = palette[dstx];
    *er = r - (dstc >> 16 & 0xff);
    *eg = g - (dstc >>  8 & 0xff);
    *eb = b - (dstc       & 0xff);
    return dstx;
}
