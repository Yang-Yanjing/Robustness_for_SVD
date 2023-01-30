 ****************************************************************************/
static subpicture_t *Filter( filter_t *p_filter, mtime_t date )
{
    filter_sys_t *p_sys = p_filter->p_sys;
    subpicture_t *p_spu = NULL;
    video_format_t fmt;
    vlc_mutex_lock( &p_sys->lock );
    if( p_sys->last_time + p_sys->i_refresh > date )
        goto out;
    if( p_sys->filepath != NULL )
    {
        char *fmt = MarqueeReadFile( p_filter, p_sys->filepath );
        if( fmt != NULL )
        {
            free( p_sys->format );
            p_sys->format = fmt;
        }
    }
    char *msg = str_format_time( p_sys->format ? p_sys->format : "" );
    if( unlikely( msg == NULL ) )
        goto out;
    if( p_sys->message != NULL && !strcmp( msg, p_sys->message ) )
    {
        free( msg );
        goto out;
    }
    free( p_sys->message );
    p_sys->message = msg;
    p_spu = filter_NewSubpicture( p_filter );
    if( !p_spu )
        goto out;
    memset( &fmt, 0, sizeof(video_format_t) );
    fmt.i_chroma = VLC_CODEC_TEXT;
    fmt.i_width = fmt.i_height = 0;
    fmt.i_x_offset = 0;
    fmt.i_y_offset = 0;
    fmt.i_sar_den = fmt.i_sar_num = 1;
    p_spu->p_region = subpicture_region_New( &fmt );
    if( !p_spu->p_region )
    {
        p_filter->pf_sub_buffer_del( p_filter, p_spu );
        p_spu = NULL;
        goto out;
    }
    p_sys->last_time = date;
    p_spu->p_region->psz_text = strdup( msg );
    p_spu->i_start = date;
    p_spu->i_stop  = p_sys->i_timeout == 0 ? 0 : date + p_sys->i_timeout * 1000;
    p_spu->b_ephemer = true;
    /*  where to locate the string: */
    if( p_sys->i_pos < 0 )
    {   /*  set to an absolute xy */
        p_spu->p_region->i_align = SUBPICTURE_ALIGN_LEFT | SUBPICTURE_ALIGN_TOP;
        p_spu->b_absolute = true;
    }
    else
    {   /* set to one of the 9 relative locations */
        p_spu->p_region->i_align = p_sys->i_pos;
        p_spu->b_absolute = false;
    }
    p_spu->p_region->i_x = p_sys->i_xoff;
    p_spu->p_region->i_y = p_sys->i_yoff;
    p_spu->p_region->p_style = text_style_Duplicate( p_sys->p_style );
out:
    vlc_mutex_unlock( &p_sys->lock );
    return p_spu;
}
