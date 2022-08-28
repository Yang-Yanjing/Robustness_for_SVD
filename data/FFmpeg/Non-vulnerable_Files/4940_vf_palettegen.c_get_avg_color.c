static uint32_t get_avg_color(struct color_ref * const *refs,
                              const struct range_box *box)
{
    int i;
    const int n = box->len;
    uint64_t r = 0, g = 0, b = 0, div = 0;
    for (i = 0; i < n; i++) {
        const struct color_ref *ref = refs[box->start + i];
        r += (ref->color >> 16 & 0xff) * ref->count;
        g += (ref->color >>  8 & 0xff) * ref->count;
        b += (ref->color       & 0xff) * ref->count;
        div += ref->count;
    }
    r = r / div;
    g = g / div;
    b = b / div;
    return 0xffU<<24 | r<<16 | g<<8 | b;
}
