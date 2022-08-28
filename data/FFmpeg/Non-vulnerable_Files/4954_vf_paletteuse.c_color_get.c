static av_always_inline int color_get(struct cache_node *cache, uint32_t color,
                                      uint8_t r, uint8_t g, uint8_t b,
                                      const struct color_node *map,
                                      const uint32_t *palette,
                                      const enum color_search_method search_method)
{
    int i;
    const uint8_t rgb[] = {r, g, b};
    const uint8_t rhash = r & ((1<<NBITS)-1);
    const uint8_t ghash = g & ((1<<NBITS)-1);
    const uint8_t bhash = b & ((1<<NBITS)-1);
    const unsigned hash = rhash<<(NBITS*2) | ghash<<NBITS | bhash;
    struct cache_node *node = &cache[hash];
    struct cached_color *e;
    for (i = 0; i < node->nb_entries; i++) {
        e = &node->entries[i];
        if (e->color == color)
            return e->pal_entry;
    }
    e = av_dynarray2_add((void**)&node->entries, &node->nb_entries,
                         sizeof(*node->entries), NULL);
    if (!e)
        return AVERROR(ENOMEM);
    e->color = color;
    e->pal_entry = COLORMAP_NEAREST(search_method, palette, map, rgb);
    return e->pal_entry;
}
