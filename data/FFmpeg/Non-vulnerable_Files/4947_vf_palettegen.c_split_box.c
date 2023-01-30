static void split_box(PaletteGenContext *s, struct range_box *box, int n)
{
    struct range_box *new_box = &s->boxes[s->nb_boxes++];
    new_box->start     = n + 1;
    new_box->len       = box->start + box->len - new_box->start;
    new_box->sorted_by = box->sorted_by;
    box->len -= new_box->len;
    av_assert0(box->len     >= 1);
    av_assert0(new_box->len >= 1);
    box->color     = get_avg_color(s->refs, box);
    new_box->color = get_avg_color(s->refs, new_box);
    box->variance     = -1;
    new_box->variance = -1;
}
