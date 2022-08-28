void ff_copy_rectangle2(FFDrawContext *draw,
                        uint8_t *dst[], int dst_linesize[],
                        uint8_t *src[], int src_linesize[],
                        int dst_x, int dst_y, int src_x, int src_y,
                        int w, int h)
{
    int plane, y, wp, hp;
    uint8_t *p, *q;
    for (plane = 0; plane < draw->nb_planes; plane++) {
        p = pointer_at(draw, src, src_linesize, plane, src_x, src_y);
        q = pointer_at(draw, dst, dst_linesize, plane, dst_x, dst_y);
        wp = FF_CEIL_RSHIFT(w, draw->hsub[plane]) * draw->pixelstep[plane];
        hp = FF_CEIL_RSHIFT(h, draw->vsub[plane]);
        for (y = 0; y < hp; y++) {
            memcpy(q, p, wp);
            p += src_linesize[plane];
            q += dst_linesize[plane];
        }
    }
}
