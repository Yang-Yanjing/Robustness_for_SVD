static int colormap_insert(struct color_node *map,
                           uint8_t *color_used,
                           int *nb_used,
                           const uint32_t *palette,
                           const struct color_rect *box)
{
    uint32_t c;
    int component, cur_id;
    int node_left_id = -1, node_right_id = -1;
    struct color_node *node;
    struct color_rect box1, box2;
    const int pal_id = get_next_color(color_used, palette, &component, box);
    if (pal_id < 0)
        return -1;
    
    cur_id = (*nb_used)++;
    c = palette[pal_id];
    node = &map[cur_id];
    node->split = component;
    node->palette_id = pal_id;
    node->val[0] = c>>16 & 0xff;
    node->val[1] = c>> 8 & 0xff;
    node->val[2] = c     & 0xff;
    color_used[pal_id] = 1;
    
    box1 = box2 = *box;
    box1.max[component] = node->val[component];
    box2.min[component] = node->val[component] + 1;
    node_left_id = colormap_insert(map, color_used, nb_used, palette, &box1);
    if (box2.min[component] <= box2.max[component])
        node_right_id = colormap_insert(map, color_used, nb_used, palette, &box2);
    node->left_id  = node_left_id;
    node->right_id = node_right_id;
    return cur_id;
}
