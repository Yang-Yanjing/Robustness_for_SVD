 *****************************************************************************/
static int Open( vlc_object_t *p_this )
{
    demux_t     *p_demux = (demux_t*)p_this;
    demux_sys_t *p_sys;
    /* Fill p_demux field */
    p_demux->pf_demux = Demux;
    p_demux->pf_control = Control;
    p_demux->p_sys = p_sys = calloc( 1, sizeof( demux_sys_t ) );
    if( !p_sys )
        return VLC_ENOMEM;
    p_sys->f_fps = var_CreateGetFloat( p_demux, "screen-fps" );
    p_sys->i_incr = 1000000 / p_sys->f_fps;;
    p_sys->i_next_date = 0;
#ifdef SCREEN_SUBSCREEN
    p_sys->i_top = var_CreateGetInteger( p_demux, "screen-top" );
    p_sys->i_left = var_CreateGetInteger( p_demux, "screen-left" );
    p_sys->i_width = var_CreateGetInteger( p_demux, "screen-width" );
    p_sys->i_height = var_CreateGetInteger( p_demux, "screen-height" );
    if( p_sys->i_width > 0 && p_sys->i_height > 0 )
        msg_Dbg( p_demux, "capturing subscreen top: %d, left: %d, "
                          "width: %d, height: %d",
                          p_sys->i_top,
                          p_sys->i_left,
                          p_sys->i_width,
                          p_sys->i_height );
#endif
#ifdef SCREEN_DISPLAY_ID
    p_sys->i_display_id = var_CreateGetInteger( p_demux, "screen-display-id" );
    p_sys->i_screen_index = var_CreateGetInteger( p_demux, "screen-index" );
#endif
    if( screen_InitCapture( p_demux ) != VLC_SUCCESS )
    {
        free( p_sys );
        return VLC_EGENERIC;
    }
    p_sys->fmt.video.i_frame_rate      = 1000000;
    p_sys->fmt.video.i_frame_rate_base = p_sys->i_incr;
    msg_Dbg( p_demux, "screen width: %i, height: %i, depth: %i",
             p_sys->fmt.video.i_width, p_sys->fmt.video.i_height,
             p_sys->fmt.video.i_bits_per_pixel );
#ifdef SCREEN_SUBSCREEN
    if( p_sys->i_width > 0 && p_sys->i_height > 0 )
    {
        if( p_sys->i_left + p_sys->i_width > p_sys->fmt.video.i_width ||
            p_sys->i_top + p_sys->i_height > p_sys->fmt.video.i_height )
        {
            msg_Err( p_demux, "subscreen region overflows the screen" );
            free( p_sys );
            return VLC_EGENERIC;
        }
        else
        {
            p_sys->i_screen_width = p_sys->fmt.video.i_width;
            p_sys->i_screen_height = p_sys->fmt.video.i_height;
            p_sys->fmt.video.i_visible_width =
            p_sys->fmt.video.i_width = p_sys->i_width;
            p_sys->fmt.video.i_visible_height =
            p_sys->fmt.video.i_height = p_sys->i_height;
            p_sys->b_follow_mouse = var_CreateGetBool( p_demux,
                                                "screen-follow-mouse" );
            if( p_sys->b_follow_mouse )
                msg_Dbg( p_demux, "mouse following enabled" );
        }
    }
#endif
#ifdef SCREEN_MOUSE
    char *mousefile = var_InheritString( p_demux, "screen-mouse-image" );
    char *mouseurl = mousefile ? vlc_path2uri( mousefile, NULL ) : NULL;
    free( mousefile );
    if( mouseurl )
    {
        image_handler_t *p_image;
        video_format_t fmt_in, fmt_out;
        msg_Dbg( p_demux, "Using %s for the mouse pointer image", mouseurl );
        memset( &fmt_in, 0, sizeof( fmt_in ) );
        memset( &fmt_out, 0, sizeof( fmt_out ) );
        fmt_out.i_chroma = VLC_CODEC_RGBA;
        p_image = image_HandlerCreate( p_demux );
        if( p_image )
        {
            p_sys->p_mouse =
                image_ReadUrl( p_image, mouseurl, &fmt_in, &fmt_out );
            image_HandlerDelete( p_image );
        }
        if( !p_sys->p_mouse )
            msg_Err( p_demux, "Failed to open mouse pointer image (%s)",
                     mouseurl );
        free( mouseurl );
    }
#endif
    p_sys->es = es_out_Add( p_demux->out, &p_sys->fmt );
    p_sys->i_start = mdate();
    return VLC_SUCCESS;
}
