}
static picture_t *Filter( filter_t *p_filter, picture_t *p_inpic )
{
    filter_sys_t *p_sys = p_filter->p_sys;
    if( !p_inpic )
        return NULL;
    picture_t *p_outpic = filter_NewPicture( p_filter );
    if( !p_outpic )
    {
        picture_Release( p_inpic );
        return NULL;
    }
    picture_Copy( p_outpic, p_inpic );
    if( !p_sys->b_old )
    {
        picture_Copy( p_sys->p_old, p_inpic );
        p_sys->b_old = true;
        goto exit;
    }
    int i_pix_offset;
    int i_pix_size;
    if( p_sys->is_yuv_planar )
    {
        PreparePlanar( p_filter, p_inpic );
        i_pix_offset = 0;
        i_pix_size = 1;
    }
    else
    {
        if( PreparePacked( p_filter, p_inpic, &i_pix_offset ) )
            goto exit;
        i_pix_size = 2;
    }
    /**
     * Get the areas where movement was detected
     */
    const video_format_t *p_fmt = &p_filter->fmt_in.video;
    p_sys->i_colors = FindShapes( p_sys->p_buf2, p_sys->p_buf, p_fmt->i_width, p_fmt->i_width, p_fmt->i_height,
                                  p_sys->colors, p_sys->color_x_min, p_sys->color_x_max, p_sys->color_y_min, p_sys->color_y_max );
    /**
     * Count final number of shapes
     * Draw rectangles (there can be more than 1 moving shape in 1 rectangle)
     */
    Draw( p_filter, &p_outpic->p[Y_PLANE].p_pixels[i_pix_offset], p_outpic->p[Y_PLANE].i_pitch, i_pix_size );
    /**
     * We're done. Lets keep a copy of the picture
     * TODO we may just picture_Release with a latency of 1 if the filters/vout
     * handle it correctly */
    picture_Copy( p_sys->p_old, p_inpic );
exit:
    picture_Release( p_inpic );
    return p_outpic;
}
