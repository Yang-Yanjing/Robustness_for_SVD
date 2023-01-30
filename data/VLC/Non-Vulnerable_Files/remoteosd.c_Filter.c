 ****************************************************************************/
static subpicture_t *Filter( filter_t *p_filter, mtime_t date )
{
    filter_sys_t *p_sys = p_filter->p_sys;
    subpicture_t *p_spu;
    subpicture_region_t *p_region;
    video_format_t fmt;
    picture_t *p_pic;
    if( !p_sys->b_need_update )
    {
        return NULL;
    }
    vlc_mutex_lock( &p_sys->lock );
    p_pic = p_sys->p_pic;
    if( !p_pic )
    {
        vlc_mutex_unlock( &p_sys->lock );
        return NULL;
    }
    /* Allocate the subpicture internal data. */
    p_spu = filter_NewSubpicture( p_filter );
    if( !p_spu )
    {
        vlc_mutex_unlock( &p_sys->lock );
        return NULL;
    }
    p_spu->b_absolute = false;
    p_spu->i_start = date;
    p_spu->i_stop = 0;
    p_spu->b_ephemer = true;
    if( !p_sys->b_continue )
        p_spu->i_stop = p_spu->i_start + 1;
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
        vlc_mutex_unlock( &p_sys->lock );
        return NULL;
    }
    /* FIXME the copy is probably not needed anymore */
    picture_Copy( p_region->p_picture, p_pic );
    p_sys->b_need_update = false;
    vlc_mutex_unlock( &p_sys->lock );
    /* set to one of the 9 relative locations */
    p_region->i_align = 0; /* Center */
    p_spu->b_absolute = false;
    p_spu->i_original_picture_width = 0; /*Let vout core do the horizontal scaling */
    p_spu->i_original_picture_height = fmt.i_height;
    p_spu->p_region = p_region;
    p_spu->i_alpha = ( p_sys->i_alpha );
    return p_spu;
}
