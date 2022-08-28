static void update_clut(LUT3DContext *lut3d, const AVFrame *frame)
{
    const uint8_t *data = frame->data[0];
    const int linesize  = frame->linesize[0];
    const int w = lut3d->clut_width;
    const int step = lut3d->clut_step;
    const uint8_t *rgba_map = lut3d->clut_rgba_map;
    const int level = lut3d->lutsize;
#define LOAD_CLUT(nbits) do {                                           \
    int i, j, k, x = 0, y = 0;                                          \
                                                                        \
    for (k = 0; k < level; k++) {                                       \
        for (j = 0; j < level; j++) {                                   \
            for (i = 0; i < level; i++) {                               \
                const uint##nbits##_t *src = (const uint##nbits##_t *)  \
                    (data + y*linesize + x*step);                       \
                struct rgbvec *vec = &lut3d->lut[i][j][k];              \
                vec->r = src[rgba_map[0]] / (float)((1<<(nbits)) - 1);  \
                vec->g = src[rgba_map[1]] / (float)((1<<(nbits)) - 1);  \
                vec->b = src[rgba_map[2]] / (float)((1<<(nbits)) - 1);  \
                if (++x == w) {                                         \
                    x = 0;                                              \
                    y++;                                                \
                }                                                       \
            }                                                           \
        }                                                               \
    }                                                                   \
} while (0)
    if (!lut3d->clut_is16bit) LOAD_CLUT(8);
    else                      LOAD_CLUT(16);
}
