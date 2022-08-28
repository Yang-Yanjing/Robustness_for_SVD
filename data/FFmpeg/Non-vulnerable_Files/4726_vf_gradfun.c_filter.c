static void filter(GradFunContext *ctx, uint8_t *dst, const uint8_t *src, int width, int height, int dst_linesize, int src_linesize, int r)
{
    int bstride = FFALIGN(width, 16) / 2;
    int y;
    uint32_t dc_factor = (1 << 21) / (r * r);
    uint16_t *dc = ctx->buf + 16;
    uint16_t *buf = ctx->buf + bstride + 32;
    int thresh = ctx->thresh;
    memset(dc, 0, (bstride + 16) * sizeof(*buf));
    for (y = 0; y < r; y++)
        ctx->blur_line(dc, buf + y * bstride, buf + (y - 1) * bstride, src + 2 * y * src_linesize, src_linesize, width / 2);
    for (;;) {
        if (y < height - r) {
            int mod = ((y + r) / 2) % r;
            uint16_t *buf0 = buf + mod * bstride;
            uint16_t *buf1 = buf + (mod ? mod - 1 : r - 1) * bstride;
            int x, v;
            ctx->blur_line(dc, buf0, buf1, src + (y + r) * src_linesize, src_linesize, width / 2);
            for (x = v = 0; x < r; x++)
                v += dc[x];
            for (; x < width / 2; x++) {
                v += dc[x] - dc[x-r];
                dc[x-r] = v * dc_factor >> 16;
            }
            for (; x < (width + r + 1) / 2; x++)
                dc[x-r] = v * dc_factor >> 16;
            for (x = -r / 2; x < 0; x++)
                dc[x] = dc[0];
        }
        if (y == r) {
            for (y = 0; y < r; y++)
                ctx->filter_line(dst + y * dst_linesize, src + y * src_linesize, dc - r / 2, width, thresh, dither[y & 7]);
        }
        ctx->filter_line(dst + y * dst_linesize, src + y * src_linesize, dc - r / 2, width, thresh, dither[y & 7]);
        if (++y >= height) break;
        ctx->filter_line(dst + y * dst_linesize, src + y * src_linesize, dc - r / 2, width, thresh, dither[y & 7]);
        if (++y >= height) break;
    }
    emms_c();
}
