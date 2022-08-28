static int parse_dat(AVFilterContext *ctx, FILE *f)
{
    LUT3DContext *lut3d = ctx->priv;
    char line[MAX_LINE_SIZE];
    int i, j, k, size;
    lut3d->lutsize = size = 33;
    NEXT_LINE(skip_line(line));
    if (!strncmp(line, "3DLUTSIZE ", 10)) {
        size = strtol(line + 10, NULL, 0);
        if (size < 2 || size > MAX_LEVEL) {
            av_log(ctx, AV_LOG_ERROR, "Too large or invalid 3D LUT size\n");
            return AVERROR(EINVAL);
        }
        lut3d->lutsize = size;
        NEXT_LINE(skip_line(line));
    }
    for (k = 0; k < size; k++) {
        for (j = 0; j < size; j++) {
            for (i = 0; i < size; i++) {
                struct rgbvec *vec = &lut3d->lut[k][j][i];
                if (k != 0 || j != 0 || i != 0)
                    NEXT_LINE(skip_line(line));
                if (sscanf(line, "%f %f %f", &vec->r, &vec->g, &vec->b) != 3)
                    return AVERROR_INVALIDDATA;
            }
        }
    }
    return 0;
}
