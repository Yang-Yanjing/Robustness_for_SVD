void ff_blend_rectangle(FFDrawContext *draw, FFDrawColor *color,
                        uint8_t *dst[], int dst_linesize[],
                        int dst_w, int dst_h,
                        int x0, int y0, int w, int h)
{
    unsigned alpha, nb_planes, nb_comp, plane, comp;
    int w_sub, h_sub, x_sub, y_sub, left, right, top, bottom, y;
    uint8_t *p0, *p;
    
    clip_interval(dst_w, &x0, &w, NULL);
    clip_interval(dst_h, &y0, &h, NULL);
    if (w <= 0 || h <= 0 || !color->rgba[3])
        return;
    
    alpha = 0x10203 * color->rgba[3] + 0x2;
    nb_planes = (draw->nb_planes - 1) | 1; 
    for (plane = 0; plane < nb_planes; plane++) {
        nb_comp = draw->pixelstep[plane];
        p0 = pointer_at(draw, dst, dst_linesize, plane, x0, y0);
        w_sub = w;
        h_sub = h;
        x_sub = x0;
        y_sub = y0;
        subsampling_bounds(draw->hsub[plane], &x_sub, &w_sub, &left, &right);
        subsampling_bounds(draw->vsub[plane], &y_sub, &h_sub, &top, &bottom);
        for (comp = 0; comp < nb_comp; comp++) {
            if (!component_used(draw, plane, comp))
                continue;
            p = p0 + comp;
            if (top) {
                blend_line(p, color->comp[plane].u8[comp], alpha >> 1,
                           draw->pixelstep[plane], w_sub,
                           draw->hsub[plane], left, right);
                p += dst_linesize[plane];
            }
            for (y = 0; y < h_sub; y++) {
                blend_line(p, color->comp[plane].u8[comp], alpha,
                           draw->pixelstep[plane], w_sub,
                           draw->hsub[plane], left, right);
                p += dst_linesize[plane];
            }
            if (bottom)
                blend_line(p, color->comp[plane].u8[comp], alpha >> 1,
                           draw->pixelstep[plane], w_sub,
                           draw->hsub[plane], left, right);
        }
    }
}
