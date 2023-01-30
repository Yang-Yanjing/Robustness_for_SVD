void ff_blend_mask(FFDrawContext *draw, FFDrawColor *color,
                   uint8_t *dst[], int dst_linesize[], int dst_w, int dst_h,
                   uint8_t *mask,  int mask_linesize, int mask_w, int mask_h,
                   int l2depth, unsigned endianness, int x0, int y0)
{
    unsigned alpha, nb_planes, nb_comp, plane, comp;
    int xm0, ym0, w_sub, h_sub, x_sub, y_sub, left, right, top, bottom, y;
    uint8_t *p0, *p, *m;
    clip_interval(dst_w, &x0, &mask_w, &xm0);
    clip_interval(dst_h, &y0, &mask_h, &ym0);
    mask += ym0 * mask_linesize;
    if (mask_w <= 0 || mask_h <= 0 || !color->rgba[3])
        return;
    

    alpha = (0x10307 * color->rgba[3] + 0x3) >> 8;
    nb_planes = (draw->nb_planes - 1) | 1; 
    for (plane = 0; plane < nb_planes; plane++) {
        nb_comp = draw->pixelstep[plane];
        p0 = pointer_at(draw, dst, dst_linesize, plane, x0, y0);
        w_sub = mask_w;
        h_sub = mask_h;
        x_sub = x0;
        y_sub = y0;
        subsampling_bounds(draw->hsub[plane], &x_sub, &w_sub, &left, &right);
        subsampling_bounds(draw->vsub[plane], &y_sub, &h_sub, &top, &bottom);
        for (comp = 0; comp < nb_comp; comp++) {
            if (!component_used(draw, plane, comp))
                continue;
            p = p0 + comp;
            m = mask;
            if (top) {
                blend_line_hv(p, draw->pixelstep[plane],
                              color->comp[plane].u8[comp], alpha,
                              m, mask_linesize, l2depth, w_sub,
                              draw->hsub[plane], draw->vsub[plane],
                              xm0, left, right, top);
                p += dst_linesize[plane];
                m += top * mask_linesize;
            }
            for (y = 0; y < h_sub; y++) {
                blend_line_hv(p, draw->pixelstep[plane],
                              color->comp[plane].u8[comp], alpha,
                              m, mask_linesize, l2depth, w_sub,
                              draw->hsub[plane], draw->vsub[plane],
                              xm0, left, right, 1 << draw->vsub[plane]);
                p += dst_linesize[plane];
                m += mask_linesize << draw->vsub[plane];
            }
            if (bottom)
                blend_line_hv(p, draw->pixelstep[plane],
                              color->comp[plane].u8[comp], alpha,
                              m, mask_linesize, l2depth, w_sub,
                              draw->hsub[plane], draw->vsub[plane],
                              xm0, left, right, bottom);
        }
    }
}
