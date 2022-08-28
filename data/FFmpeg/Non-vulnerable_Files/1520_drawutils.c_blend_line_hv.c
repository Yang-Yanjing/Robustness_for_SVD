static void blend_line_hv(uint8_t *dst, int dst_delta,
                          unsigned src, unsigned alpha,
                          uint8_t *mask, int mask_linesize, int l2depth, int w,
                          unsigned hsub, unsigned vsub,
                          int xm, int left, int right, int hband)
{
    int x;
    if (left) {
        blend_pixel(dst, src, alpha, mask, mask_linesize, l2depth,
                    left, hband, hsub + vsub, xm);
        dst += dst_delta;
        xm += left;
    }
    for (x = 0; x < w; x++) {
        blend_pixel(dst, src, alpha, mask, mask_linesize, l2depth,
                    1 << hsub, hband, hsub + vsub, xm);
        dst += dst_delta;
        xm += 1 << hsub;
    }
    if (right)
        blend_pixel(dst, src, alpha, mask, mask_linesize, l2depth,
                    right, hband, hsub + vsub, xm);
}
