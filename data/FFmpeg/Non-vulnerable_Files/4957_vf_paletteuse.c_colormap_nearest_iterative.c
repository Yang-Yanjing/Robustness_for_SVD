static av_always_inline uint8_t colormap_nearest_iterative(const struct color_node *root, const uint8_t *target)
{
    int pos = 0, best_node_id = -1, best_dist = INT_MAX, cur_color_id = 0;
    struct stack_node nodes[16];
    struct stack_node *node = &nodes[0];
    for (;;) {
        const struct color_node *kd = &root[cur_color_id];
        const uint8_t *current = kd->val;
        const int current_to_target = diff(target, current);
        

        if (current_to_target < best_dist) {
            best_node_id = cur_color_id;
            if (!current_to_target)
                goto end; 
            best_dist = current_to_target;
        }
        
        if (kd->left_id != -1 || kd->right_id != -1) {
            const int split = kd->split;
            const int dx = target[split] - current[split];
            int nearer_kd_id, further_kd_id;
            
            if (dx <= 0) nearer_kd_id = kd->left_id,  further_kd_id = kd->right_id;
            else         nearer_kd_id = kd->right_id, further_kd_id = kd->left_id;
            if (nearer_kd_id != -1) {
                if (further_kd_id != -1) {
                    

                    node->color_id = further_kd_id;
                    node->dx2 = dx*dx;
                    pos++;
                    node++;
                }
                


                cur_color_id = nearer_kd_id;
                continue;
            } else if (dx*dx < best_dist) {
                



                cur_color_id = further_kd_id;
                continue;
            }
        }
        

        do {
            if (--pos < 0)
                goto end;
            node--;
        } while (node->dx2 >= best_dist);
        

        cur_color_id = node->color_id;
    }
end:
    return root[best_node_id].palette_id;
}
