static int debug_accuracy(const struct color_node *node, const uint32_t *palette,
                          const enum color_search_method search_method)
{
    int r, g, b, ret = 0;
    for (r = 0; r < 256; r++) {
        for (g = 0; g < 256; g++) {
            for (b = 0; b < 256; b++) {
                const uint8_t rgb[] = {r, g, b};
                const int r1 = COLORMAP_NEAREST(search_method, palette, node, rgb);
                const int r2 = colormap_nearest_bruteforce(palette, rgb);
                if (r1 != r2) {
                    const uint32_t c1 = palette[r1];
                    const uint32_t c2 = palette[r2];
                    const uint8_t palrgb1[] = { c1>>16 & 0xff, c1>> 8 & 0xff, c1 & 0xff };
                    const uint8_t palrgb2[] = { c2>>16 & 0xff, c2>> 8 & 0xff, c2 & 0xff };
                    const int d1 = diff(palrgb1, rgb);
                    const int d2 = diff(palrgb2, rgb);
                    if (d1 != d2) {
                        av_log(NULL, AV_LOG_ERROR,
                               "/!\\ %02X%02X%02X: %d ! %d (%06X ! %06X) / dist: %d ! %d\n",
                               r, g, b, r1, r2, c1 & 0xffffff, c2 & 0xffffff, d1, d2);
                        ret = 1;
                    }
                }
            }
        }
    }
    return ret;
}
