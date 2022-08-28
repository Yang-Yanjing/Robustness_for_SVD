static av_always_inline uint8_t colormap_nearest_recursive(const struct color_node *node, const uint8_t *rgb)
{
    struct nearest_color res = {.dist_sqd = INT_MAX, .node_pos = -1};
    colormap_nearest_node(node, 0, rgb, &res);
    return node[res.node_pos].palette_id;
}
