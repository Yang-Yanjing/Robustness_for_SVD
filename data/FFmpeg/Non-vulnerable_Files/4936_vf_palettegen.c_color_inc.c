static int color_inc(struct hist_node *hist, uint32_t color)
{
    int i;
    const unsigned hash = color_hash(color);
    struct hist_node *node = &hist[hash];
    struct color_ref *e;
    for (i = 0; i < node->nb_entries; i++) {
        e = &node->entries[i];
        if (e->color == color) {
            e->count++;
            return 0;
        }
    }
    e = av_dynarray2_add((void**)&node->entries, &node->nb_entries,
                         sizeof(*node->entries), NULL);
    if (!e)
        return AVERROR(ENOMEM);
    e->color = color;
    e->count = 1;
    return 1;
}
