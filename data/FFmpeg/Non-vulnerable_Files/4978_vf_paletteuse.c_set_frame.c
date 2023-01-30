static av_always_inline int set_frame(PaletteUseContext *s, AVFrame *out, AVFrame *in,
                                      int x_start, int y_start, int w, int h,
                                      enum dithering_mode dither,
                                      const enum color_search_method search_method)
{
    int x, y;
    const struct color_node *map = s->map;
    struct cache_node *cache = s->cache;
    const uint32_t *palette = s->palette;
    const int src_linesize = in ->linesize[0] >> 2;
    const int dst_linesize = out->linesize[0];
    uint32_t *src = ((uint32_t *)in ->data[0]) + y_start*src_linesize;
    uint8_t  *dst =              out->data[0]  + y_start*dst_linesize;
    w += x_start;
    h += y_start;
    for (y = y_start; y < h; y++) {
        for (x = x_start; x < w; x++) {
            int er, eg, eb;
            if (dither == DITHERING_BAYER) {
                const int d = s->ordered_dither[(y & 7)<<3 | (x & 7)];
                const uint8_t r8 = src[x] >> 16 & 0xff;
                const uint8_t g8 = src[x] >>  8 & 0xff;
                const uint8_t b8 = src[x]       & 0xff;
                const uint8_t r = av_clip_uint8(r8 + d);
                const uint8_t g = av_clip_uint8(g8 + d);
                const uint8_t b = av_clip_uint8(b8 + d);
                const uint32_t c = r<<16 | g<<8 | b;
                const int color = color_get(cache, c, r, g, b, map, palette, search_method);
                if (color < 0)
                    return color;
                dst[x] = color;
            } else if (dither == DITHERING_HECKBERT) {
                const int right = x < w - 1, down = y < h - 1;
                const int color = get_dst_color_err(cache, src[x], map, palette, &er, &eg, &eb, search_method);
                if (color < 0)
                    return color;
                dst[x] = color;
                if (right)         src[               x + 1] = dither_color(src[               x + 1], er, eg, eb, 3, 3);
                if (         down) src[src_linesize + x    ] = dither_color(src[src_linesize + x    ], er, eg, eb, 3, 3);
                if (right && down) src[src_linesize + x + 1] = dither_color(src[src_linesize + x + 1], er, eg, eb, 2, 3);
            } else if (dither == DITHERING_FLOYD_STEINBERG) {
                const int right = x < w - 1, down = y < h - 1, left = x > x_start;
                const int color = get_dst_color_err(cache, src[x], map, palette, &er, &eg, &eb, search_method);
                if (color < 0)
                    return color;
                dst[x] = color;
                if (right)         src[               x + 1] = dither_color(src[               x + 1], er, eg, eb, 7, 4);
                if (left  && down) src[src_linesize + x - 1] = dither_color(src[src_linesize + x - 1], er, eg, eb, 3, 4);
                if (         down) src[src_linesize + x    ] = dither_color(src[src_linesize + x    ], er, eg, eb, 5, 4);
                if (right && down) src[src_linesize + x + 1] = dither_color(src[src_linesize + x + 1], er, eg, eb, 1, 4);
            } else if (dither == DITHERING_SIERRA2) {
                const int right  = x < w - 1, down  = y < h - 1, left  = x > x_start;
                const int right2 = x < w - 2,                    left2 = x > x_start + 1;
                const int color = get_dst_color_err(cache, src[x], map, palette, &er, &eg, &eb, search_method);
                if (color < 0)
                    return color;
                dst[x] = color;
                if (right)          src[                 x + 1] = dither_color(src[                 x + 1], er, eg, eb, 4, 4);
                if (right2)         src[                 x + 2] = dither_color(src[                 x + 2], er, eg, eb, 3, 4);
                if (down) {
                    if (left2)      src[  src_linesize + x - 2] = dither_color(src[  src_linesize + x - 2], er, eg, eb, 1, 4);
                    if (left)       src[  src_linesize + x - 1] = dither_color(src[  src_linesize + x - 1], er, eg, eb, 2, 4);
                                    src[  src_linesize + x    ] = dither_color(src[  src_linesize + x    ], er, eg, eb, 3, 4);
                    if (right)      src[  src_linesize + x + 1] = dither_color(src[  src_linesize + x + 1], er, eg, eb, 2, 4);
                    if (right2)     src[  src_linesize + x + 2] = dither_color(src[  src_linesize + x + 2], er, eg, eb, 1, 4);
                }
            } else if (dither == DITHERING_SIERRA2_4A) {
                const int right = x < w - 1, down = y < h - 1, left = x > x_start;
                const int color = get_dst_color_err(cache, src[x], map, palette, &er, &eg, &eb, search_method);
                if (color < 0)
                    return color;
                dst[x] = color;
                if (right)         src[               x + 1] = dither_color(src[               x + 1], er, eg, eb, 2, 2);
                if (left  && down) src[src_linesize + x - 1] = dither_color(src[src_linesize + x - 1], er, eg, eb, 1, 2);
                if (         down) src[src_linesize + x    ] = dither_color(src[src_linesize + x    ], er, eg, eb, 1, 2);
            } else {
                const uint8_t r = src[x] >> 16 & 0xff;
                const uint8_t g = src[x] >>  8 & 0xff;
                const uint8_t b = src[x]       & 0xff;
                const int color = color_get(cache, src[x] & 0xffffff, r, g, b, map, palette, search_method);
                if (color < 0)
                    return color;
                dst[x] = color;
            }
        }
        src += src_linesize;
        dst += dst_linesize;
    }
    return 0;
}
