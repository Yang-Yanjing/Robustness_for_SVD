void ff_fill_rectangle(FFDrawContext *draw, FFDrawColor *color,
                       uint8_t *dst[], int dst_linesize[],
                       int dst_x, int dst_y, int w, int h)
{
    int plane, x, y, wp, hp;
    uint8_t *p0, *p;
    for (plane = 0; plane < draw->nb_planes; plane++) {
        p0 = pointer_at(draw, dst, dst_linesize, plane, dst_x, dst_y);
        wp = FF_CEIL_RSHIFT(w, draw->hsub[plane]);
        hp = FF_CEIL_RSHIFT(h, draw->vsub[plane]);
        if (!hp)
            return;
        p = p0;
        
        for (x = 0; x < wp; x++) {
            memcpy(p, color->comp[plane].u8, draw->pixelstep[plane]);
            p += draw->pixelstep[plane];
        }
        wp *= draw->pixelstep[plane];
        
        p = p0 + dst_linesize[plane];
        for (y = 1; y < hp; y++) {
            memcpy(p, p0, wp);
            p += dst_linesize[plane];
        }
    }
}
