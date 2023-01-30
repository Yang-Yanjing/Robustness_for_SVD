/* This should probably be moved to subpictures.c afterward */
static subpicture_region_t* subpicture_region_Clone(subpicture_region_t *p_region_src)
{
    if (!p_region_src)
        return NULL;
    subpicture_region_t *p_region_dst = subpicture_region_New(&p_region_src->fmt);
    if (unlikely(!p_region_dst))
        return NULL;
    p_region_dst->i_x      = p_region_src->i_x;
    p_region_dst->i_y      = p_region_src->i_y;
    p_region_dst->i_align  = p_region_src->i_align;
    p_region_dst->i_alpha  = p_region_src->i_alpha;
    p_region_dst->psz_text = p_region_src->psz_text ? strdup(p_region_src->psz_text) : NULL;
    p_region_dst->psz_html = p_region_src->psz_html ? strdup(p_region_src->psz_html) : NULL;
    if (p_region_src->p_style != NULL) {
        p_region_dst->p_style = malloc(sizeof(*p_region_dst->p_style));
        p_region_dst->p_style = text_style_Copy(p_region_dst->p_style,
                                                p_region_src->p_style);
    }
    //Palette is already copied by subpicture_region_New, we just have to duplicate p_pixels
    for (int i = 0; i < p_region_src->p_picture->i_planes; i++)
        memcpy(p_region_dst->p_picture->p[i].p_pixels,
               p_region_src->p_picture->p[i].p_pixels,
               p_region_src->p_picture->p[i].i_lines * p_region_src->p_picture->p[i].i_pitch);
    return p_region_dst;
}
