static void extract_from_packed(uint8_t *dst, int dst_linesize,
                                const uint8_t *src, int src_linesize,
                                int width, int height,
                                int depth, int step, int comp)
{
    int x, y;
    for (y = 0; y < height; y++) {
        switch (depth) {
        case 1:
            for (x = 0; x < width; x++)
                dst[x] = src[x * step + comp];
            break;
        case 2:
            for (x = 0; x < width; x++) {
                dst[x * 2    ] = src[x * step + comp * 2    ];
                dst[x * 2 + 1] = src[x * step + comp * 2 + 1];
            }
            break;
        }
        dst += dst_linesize;
        src += src_linesize;
    }
}
}
static void extract_from_packed(uint8_t *dst, int dst_linesize,
                                const uint8_t *src, int src_linesize,
                                int width, int height,
                                int depth, int step, int comp)
{
    int x, y;
    for (y = 0; y < height; y++) {
        switch (depth) {
        case 1:
            for (x = 0; x < width; x++)
                dst[x] = src[x * step + comp];
            break;
        case 2:
            for (x = 0; x < width; x++) {
                dst[x * 2    ] = src[x * step + comp * 2    ];
                dst[x * 2 + 1] = src[x * step + comp * 2 + 1];
            }
            break;
        }
