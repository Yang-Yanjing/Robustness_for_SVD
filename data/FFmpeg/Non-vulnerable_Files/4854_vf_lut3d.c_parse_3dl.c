static int parse_3dl(AVFilterContext *ctx, FILE *f)
{
    char line[MAX_LINE_SIZE];
    LUT3DContext *lut3d = ctx->priv;
    int i, j, k;
    const int size = 17;
    const float scale = 16*16*16;
    lut3d->lutsize = size;
    NEXT_LINE(skip_line(line));
    for (k = 0; k < size; k++) {
        for (j = 0; j < size; j++) {
            for (i = 0; i < size; i++) {
                int r, g, b;
                struct rgbvec *vec = &lut3d->lut[k][j][i];
                NEXT_LINE(skip_line(line));
                if (sscanf(line, "%d %d %d", &r, &g, &b) != 3)
                    return AVERROR_INVALIDDATA;
                vec->r = r / scale;
                vec->g = g / scale;
                vec->b = b / scale;
            }
        }
    }
    return 0;
}
