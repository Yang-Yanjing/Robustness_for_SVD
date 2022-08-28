static struct color_ref **load_color_refs(const struct hist_node *hist, int nb_refs)
{
    int i, j, k = 0;
    struct color_ref **refs = av_malloc_array(nb_refs, sizeof(*refs));
    if (!refs)
        return NULL;
    for (j = 0; j < HIST_SIZE; j++) {
        const struct hist_node *node = &hist[j];
        for (i = 0; i < node->nb_entries; i++)
            refs[k++] = &node->entries[i];
    }
    return refs;
}
