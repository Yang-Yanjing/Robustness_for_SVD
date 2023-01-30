 */
static subpicture_t *FilterSub( filter_t *p_filter, mtime_t date )
{
    filter_sys_t *p_sys = p_filter->p_sys;
    logo_list_t *p_list = &p_sys->list;
    subpicture_t *p_spu;
    subpicture_region_t *p_region;
    video_format_t fmt;
    picture_t *p_pic;
    logo_t *p_logo;
    vlc_mutex_lock( &p_sys->lock );
    /* Basic test:  b_spu_update occurs on a dynamic change,
                    & i_next_pic is the general timer, when to
                    look at updating the logo image */
    if( ( !p_sys->b_spu_update && p_list->i_next_pic > date ) ||
        !p_list->i_repeat )
    {
        vlc_mutex_unlock( &p_sys->lock );
        return NULL;
    }
    /* adjust index to the next logo */
    p_logo = LogoListNext( p_list, date );
    p_sys->b_spu_update = false;
    p_pic = p_logo->p_pic;
    /* Allocate the subpicture internal data. */
    p_spu = filter_NewSubpicture( p_filter );
    if( !p_spu )
        goto exit;
    p_spu->b_absolute = p_sys->b_absolute;
    p_spu->i_start = date;
    p_spu->i_stop = 0;
    p_spu->b_ephemer = true;
    /* Send an empty subpicture to clear the display when needed */
    if( p_list->i_repeat != -1 && p_list->i_counter == 0 )
    {
        p_list->i_repeat--;
        if( p_list->i_repeat < 0 )
            goto exit;
    }
    if( !p_pic || !p_logo->i_alpha ||
        ( p_logo->i_alpha == -1 && !p_list->i_alpha ) )
        goto exit;
    /* Create new SPU region */
    memset( &fmt, 0, sizeof(video_format_t) );
    fmt.i_chroma = VLC_CODEC_YUVA;
    fmt.i_sar_num = fmt.i_sar_den = 1;
    fmt.i_width = fmt.i_visible_width = p_pic->p[Y_PLANE].i_visible_pitch;
    fmt.i_height = fmt.i_visible_height = p_pic->p[Y_PLANE].i_visible_lines;
    fmt.i_x_offset = fmt.i_y_offset = 0;
    p_region = subpicture_region_New( &fmt );
    if( !p_region )
    {
        msg_Err( p_filter, "cannot allocate SPU region" );
        p_filter->pf_sub_buffer_del( p_filter, p_spu );
        p_spu = NULL;
        goto exit;
    }
    /* */
    picture_Copy( p_region->p_picture, p_pic );
    /*  where to locate the logo: */
    if( p_sys->i_pos < 0 )
    {   /*  set to an absolute xy */
        p_region->i_align = SUBPICTURE_ALIGN_RIGHT | SUBPICTURE_ALIGN_TOP;
        p_spu->b_absolute = true;
    }
    else
    {   /* set to one of the 9 relative locations */
        p_region->i_align = p_sys->i_pos;
        p_spu->b_absolute = false;
    }
    p_region->i_x = p_sys->i_pos_x;
    p_region->i_y = p_sys->i_pos_y;
    p_spu->p_region = p_region;
    p_spu->i_alpha = ( p_logo->i_alpha != -1 ?
                       p_logo->i_alpha : p_list->i_alpha );
exit:
    vlc_mutex_unlock( &p_sys->lock );
    return p_spu;
}
