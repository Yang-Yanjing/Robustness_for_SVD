static int get_next_box_id_to_split(PaletteGenContext *s)
{
    int box_id, i, best_box_id = -1;
    int64_t max_variance = -1;
    if (s->nb_boxes == s->max_colors - s->reserve_transparent)
        return -1;
    for (box_id = 0; box_id < s->nb_boxes; box_id++) {
        struct range_box *box = &s->boxes[box_id];
        if (s->boxes[box_id].len >= 2) {
            if (box->variance == -1) {
                int64_t variance = 0;
                for (i = 0; i < box->len; i++) {
                    const struct color_ref *ref = s->refs[box->start + i];
                    variance += diff(ref->color, box->color) * ref->count;
                }
                box->variance = variance;
            }
            if (box->variance > max_variance) {
                best_box_id = box_id;
                max_variance = box->variance;
            }
        } else {
            box->variance = -1;
        }
    }
    return best_box_id;
}
