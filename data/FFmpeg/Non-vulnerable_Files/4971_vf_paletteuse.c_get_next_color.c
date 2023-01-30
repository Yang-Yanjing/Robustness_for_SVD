static int get_next_color(const uint8_t *color_used, const uint32_t *palette,
                          int *component, const struct color_rect *box)
{
    int wr, wg, wb;
    int i, longest = 0;
    unsigned nb_color = 0;
    struct color_rect ranges;
    struct color tmp_pal[256];
    cmp_func cmpf;
    ranges.min[0] = ranges.min[1] = ranges.min[2] = 0xff;
    ranges.max[0] = ranges.max[1] = ranges.max[2] = 0x00;
    for (i = 0; i < AVPALETTE_COUNT; i++) {
        const uint32_t c = palette[i];
        const uint8_t r = c >> 16 & 0xff;
        const uint8_t g = c >>  8 & 0xff;
        const uint8_t b = c       & 0xff;
        if (color_used[i] ||
            r < box->min[0] || g < box->min[1] || b < box->min[2] ||
            r > box->max[0] || g > box->max[1] || b > box->max[2])
            continue;
        if (r < ranges.min[0]) ranges.min[0] = r;
        if (g < ranges.min[1]) ranges.min[1] = g;
        if (b < ranges.min[2]) ranges.min[2] = b;
        if (r > ranges.max[0]) ranges.max[0] = r;
        if (g > ranges.max[1]) ranges.max[1] = g;
        if (b > ranges.max[2]) ranges.max[2] = b;
        tmp_pal[nb_color].value  = c;
        tmp_pal[nb_color].pal_id = i;
        nb_color++;
    }
    if (!nb_color)
        return -1;
    
    wr = ranges.max[0] - ranges.min[0];
    wg = ranges.max[1] - ranges.min[1];
    wb = ranges.max[2] - ranges.min[2];
    if (wr >= wg && wr >= wb) longest = 0;
    if (wg >= wr && wg >= wb) longest = 1;
    if (wb >= wr && wb >= wg) longest = 2;
    cmpf = cmp_funcs[longest];
    *component = longest;
    
    AV_QSORT(tmp_pal, nb_color, struct color, cmpf);
    return tmp_pal[nb_color >> 1].pal_id;
}
