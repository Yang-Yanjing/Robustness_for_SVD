static av_always_inline uint8_t colormap_nearest_bruteforce(const uint32_t *palette, const uint8_t *rgb)
{
    int i, pal_id = -1, min_dist = INT_MAX;
    for (i = 0; i < AVPALETTE_COUNT; i++) {
        const uint32_t c = palette[i];
        if ((c & 0xff000000) == 0xff000000) { 
            const uint8_t palrgb[] = {
                palette[i]>>16 & 0xff,
                palette[i]>> 8 & 0xff,
                palette[i]     & 0xff,
            };
            const int d = diff(palrgb, rgb);
            if (d < min_dist) {
                pal_id = i;
                min_dist = d;
            }
        }
    }
    return pal_id;
}
