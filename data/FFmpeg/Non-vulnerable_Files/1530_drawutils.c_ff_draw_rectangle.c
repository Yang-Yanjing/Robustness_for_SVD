void ff_draw_rectangle(uint8_t *dst[4], int dst_linesize[4],
                       uint8_t *src[4], int pixelstep[4],
                       int hsub, int vsub, int x, int y, int w, int h)
{
    int i, plane;
    uint8_t *p;
    for (plane = 0; plane < 4 && dst[plane]; plane++) {
        int hsub1 = plane == 1 || plane == 2 ? hsub : 0;
        int vsub1 = plane == 1 || plane == 2 ? vsub : 0;
        int width  = FF_CEIL_RSHIFT(w, hsub1);
        int height = FF_CEIL_RSHIFT(h, vsub1);
        p = dst[plane] + (y >> vsub1) * dst_linesize[plane];
        for (i = 0; i < height; i++) {
            memcpy(p + (x >> hsub1) * pixelstep[plane],
                   src[plane], width * pixelstep[plane]);
            p += dst_linesize[plane];
        }
    }
}
