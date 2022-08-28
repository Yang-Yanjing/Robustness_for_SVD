static void colormap_nearest_node(const struct color_node *map,
                                  const int node_pos,
                                  const uint8_t *target,
                                  struct nearest_color *nearest)
{
    const struct color_node *kd = map + node_pos;
    const int s = kd->split;
    int dx, nearer_kd_id, further_kd_id;
    const uint8_t *current = kd->val;
    const int current_to_target = diff(target, current);
    if (current_to_target < nearest->dist_sqd) {
        nearest->node_pos = node_pos;
        nearest->dist_sqd = current_to_target;
    }
    if (kd->left_id != -1 || kd->right_id != -1) {
        dx = target[s] - current[s];
        if (dx <= 0) nearer_kd_id = kd->left_id,  further_kd_id = kd->right_id;
        else         nearer_kd_id = kd->right_id, further_kd_id = kd->left_id;
        if (nearer_kd_id != -1)
            colormap_nearest_node(map, nearer_kd_id, target, nearest);
        if (further_kd_id != -1 && dx*dx < nearest->dist_sqd)
            colormap_nearest_node(map, further_kd_id, target, nearest);
    }
}
