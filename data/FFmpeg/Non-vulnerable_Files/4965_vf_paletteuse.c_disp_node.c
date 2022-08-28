static void disp_node(AVBPrint *buf,
                      const struct color_node *map,
                      int parent_id, int node_id,
                      int depth)
{
    const struct color_node *node = &map[node_id];
    const uint32_t fontcolor = node->val[0] > 0x50 &&
                               node->val[1] > 0x50 &&
                               node->val[2] > 0x50 ? 0 : 0xffffff;
    av_bprintf(buf, "%*cnode%d ["
               "label=\"%c%02X%c%02X%c%02X%c\" "
               "fillcolor=\"#%02x%02x%02x\" "
               "fontcolor=\"#%06X\"]\n",
               depth*INDENT, ' ', node->palette_id,
               "[  "[node->split], node->val[0],
               "][ "[node->split], node->val[1],
               " ]["[node->split], node->val[2],
               "  ]"[node->split],
               node->val[0], node->val[1], node->val[2],
               fontcolor);
    if (parent_id != -1)
        av_bprintf(buf, "%*cnode%d -> node%d\n", depth*INDENT, ' ',
                   map[parent_id].palette_id, node->palette_id);
    if (node->left_id  != -1) disp_node(buf, map, node_id, node->left_id,  depth + 1);
    if (node->right_id != -1) disp_node(buf, map, node_id, node->right_id, depth + 1);
}
