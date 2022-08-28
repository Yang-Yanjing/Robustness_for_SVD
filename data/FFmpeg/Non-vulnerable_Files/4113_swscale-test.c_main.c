int main(int argc, char **argv)
{
    enum AVPixelFormat srcFormat = AV_PIX_FMT_NONE;
    enum AVPixelFormat dstFormat = AV_PIX_FMT_NONE;
    uint8_t *rgb_data   = av_malloc(W * H * 4);
    const uint8_t * const rgb_src[4] = { rgb_data, NULL, NULL, NULL };
    int rgb_stride[4]   = { 4 * W, 0, 0, 0 };
    uint8_t *data       = av_malloc(4 * W * H);
    uint8_t *src[4]     = { data, data + W * H, data + W * H * 2, data + W * H * 3 };
    int stride[4]       = { W, W, W, W };
    int x, y;
    struct SwsContext *sws;
    AVLFG rand;
    int res = -1;
    int i;
    FILE *fp = NULL;
    if (!rgb_data || !data)
        return -1;
    for (i = 1; i < argc; i += 2) {
        if (argv[i][0] != '-' || i + 1 == argc)
            goto bad_option;
        if (!strcmp(argv[i], "-ref")) {
            fp = fopen(argv[i + 1], "r");
            if (!fp) {
                fprintf(stderr, "could not open '%s'\n", argv[i + 1]);
                goto error;
            }
        } else if (!strcmp(argv[i], "-src")) {
            srcFormat = av_get_pix_fmt(argv[i + 1]);
            if (srcFormat == AV_PIX_FMT_NONE) {
                fprintf(stderr, "invalid pixel format %s\n", argv[i + 1]);
                return -1;
            }
        } else if (!strcmp(argv[i], "-dst")) {
            dstFormat = av_get_pix_fmt(argv[i + 1]);
            if (dstFormat == AV_PIX_FMT_NONE) {
                fprintf(stderr, "invalid pixel format %s\n", argv[i + 1]);
                return -1;
            }
        } else {
bad_option:
            fprintf(stderr, "bad option or argument missing (%s)\n", argv[i]);
            goto error;
        }
    }
    sws = sws_getContext(W / 12, H / 12, AV_PIX_FMT_RGB32, W, H,
                         AV_PIX_FMT_YUVA420P, SWS_BILINEAR, NULL, NULL, NULL);
    av_lfg_init(&rand, 1);
    for (y = 0; y < H; y++)
        for (x = 0; x < W * 4; x++)
            rgb_data[ x + y * 4 * W] = av_lfg_get(&rand);
    sws_scale(sws, rgb_src, rgb_stride, 0, H / 12, src, stride);
    sws_freeContext(sws);
    av_free(rgb_data);
    if(fp) {
        res = fileTest(src, stride, W, H, fp, srcFormat, dstFormat);
        fclose(fp);
    } else {
        selfTest(src, stride, W, H, srcFormat, dstFormat);
        res = 0;
    }
error:
    av_free(data);
    return res;
}
