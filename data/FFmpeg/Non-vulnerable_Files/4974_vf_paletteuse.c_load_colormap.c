static void load_colormap(PaletteUseContext *s)
{
    int i, nb_used = 0;
    uint8_t color_used[AVPALETTE_COUNT] = {0};
    uint32_t last_color = 0;
    struct color_rect box;
    
    qsort(s->palette, AVPALETTE_COUNT, sizeof(*s->palette), cmp_pal_entry);
    for (i = 0; i < AVPALETTE_COUNT; i++) {
        const uint32_t c = s->palette[i];
        if (i != 0 && c == last_color) {
            color_used[i] = 1;
            continue;
        }
        last_color = c;
        if ((c & 0xff000000) != 0xff000000) {
            color_used[i] = 1; 
            continue;
        }
    }
    box.min[0] = box.min[1] = box.min[2] = 0x00;
    box.max[0] = box.max[1] = box.max[2] = 0xff;
    colormap_insert(s->map, color_used, &nb_used, s->palette, &box);
    if (s->dot_filename)
        disp_tree(s->map, s->dot_filename);
    if (s->debug_accuracy) {
        if (!debug_accuracy(s->map, s->palette, s->color_search_method))
            av_log(NULL, AV_LOG_INFO, "Accuracy check passed\n");
    }
}
