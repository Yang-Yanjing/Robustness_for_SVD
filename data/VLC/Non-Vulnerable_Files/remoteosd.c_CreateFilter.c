 *****************************************************************************/
static int CreateFilter ( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t *)p_this;
    filter_sys_t *p_sys = NULL;
    msg_Dbg( p_filter, "Creating vnc osd filter..." );
    p_filter->p_sys = p_sys = calloc( 1, sizeof(*p_sys) );
    if( !p_filter->p_sys )
        return VLC_ENOMEM;
    /* Populating struct */
    vlc_mutex_init( &p_sys->lock );
    p_sys->b_continue = true;
    p_sys->i_socket = -1;
    p_sys->p_pic = NULL;
    p_sys->psz_host = var_CreateGetString( p_this, RMTOSD_CFG "host" );
    if( EMPTY_STR(p_sys->psz_host) )
    {
        msg_Err( p_filter, "unable to get vnc host" );
        goto error;
    }
    p_sys->psz_passwd = var_CreateGetString( p_this, RMTOSD_CFG "password" );
    if( !p_sys->psz_passwd )
    {
        msg_Err( p_filter, "unable to get vnc password" );
        goto error;
    }
    p_sys->i_port = var_CreateGetIntegerCommand( p_this, RMTOSD_CFG "port" );
    p_sys->i_alpha = var_CreateGetIntegerCommand( p_this, RMTOSD_CFG "alpha" );
    /* in milliseconds, 0 disables polling, should not be lower than 100 */
    p_sys->i_vnc_poll_interval  = var_CreateGetIntegerCommand( p_this,
                                                       RMTOSD_CFG "update" );
    if ( p_sys->i_vnc_poll_interval < 100)
    {
       p_sys->i_vnc_poll_interval = 100;
    }
    for ( int i = 0; i < 256; i++ )
    {
        p_sys->ar_color_table_yuv[i][0] = 255;
        p_sys->ar_color_table_yuv[i][1] = 255;
        p_sys->ar_color_table_yuv[i][2] = 255;
        p_sys->ar_color_table_yuv[i][3] = 255;
    }
    p_sys->b_vnc_poll = var_CreateGetBoolCommand( p_this,
                                            RMTOSD_CFG "vnc-polling" );
    p_sys->b_vnc_mouse_events = var_CreateGetBoolCommand( p_this,
                                            RMTOSD_CFG "mouse-events" );
    p_sys->b_vnc_key_events = var_CreateGetBoolCommand( p_this,
                                            RMTOSD_CFG "key-events" );
    /* Keep track of OSD Events */
    p_sys->b_need_update  = false;
    /* Attach subpicture source callback */
    p_filter->pf_sub_source = Filter;
    p_filter->pf_sub_mouse  = MouseEvent;
    var_AddCallback( p_filter->p_libvlc, "key-pressed", KeyEvent, p_this );
    es_format_Init( &p_filter->fmt_out, SPU_ES, VLC_CODEC_SPU );
    p_filter->fmt_out.i_priority = ES_PRIORITY_SELECTABLE_MIN;
    vlc_gcrypt_init();
    /* create the vnc worker thread */
    if( vlc_clone( &p_sys->worker_thread,
                   vnc_worker_thread, p_filter, VLC_THREAD_PRIORITY_LOW ) )
    {
        msg_Err( p_filter, "cannot spawn vnc message reader thread" );
        goto error;
    }
    msg_Dbg( p_filter, "osdvnc filter started" );
    return VLC_SUCCESS;
error:
    msg_Err( p_filter, "osdvnc filter discarded" );
    stop_osdvnc( p_filter );
    vlc_mutex_destroy( &p_sys->lock );
    free( p_sys->psz_host );
    free( p_sys->psz_passwd );
    free( p_sys );
    return VLC_EGENERIC;
}
