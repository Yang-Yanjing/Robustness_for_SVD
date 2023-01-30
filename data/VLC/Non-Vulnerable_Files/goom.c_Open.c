 *****************************************************************************/
static int Open( vlc_object_t *p_this )
{
    filter_t       *p_filter = (filter_t *)p_this;
    filter_sys_t   *p_sys;
    goom_thread_t  *p_thread;
    video_format_t fmt;
    /* Allocate structure */
    p_sys = p_filter->p_sys = malloc( sizeof( filter_sys_t ) );
    /* Create goom thread */
    p_sys->p_thread = p_thread = calloc( 1, sizeof(*p_thread) );
    const int width  = p_thread->i_width  = var_InheritInteger( p_filter, "goom-width" );
    const int height = p_thread->i_height = var_InheritInteger( p_filter, "goom-height" );
    memset( &fmt, 0, sizeof(video_format_t) );
    fmt.i_width = fmt.i_visible_width = width;
    fmt.i_height = fmt.i_visible_height = height;
    fmt.i_chroma = VLC_CODEC_RGB32;
    fmt.i_sar_num = fmt.i_sar_den = 1;
    p_thread->p_vout = aout_filter_RequestVout( p_filter, NULL, &fmt );
    if( p_thread->p_vout == NULL )
    {
        msg_Err( p_filter, "no suitable vout module" );
        free( p_thread );
        free( p_sys );
        return VLC_EGENERIC;
    }
    p_thread->i_speed = MAX_SPEED - var_InheritInteger( p_filter, "goom-speed" );
    if( p_thread->i_speed < 0 )
        p_thread->i_speed = 0;
    vlc_mutex_init( &p_thread->lock );
    vlc_cond_init( &p_thread->wait );
    p_thread->i_blocks = 0;
    date_Init( &p_thread->date, p_filter->fmt_in.audio.i_rate, 1 );
    date_Set( &p_thread->date, 0 );
    p_thread->i_channels = aout_FormatNbChannels( &p_filter->fmt_in.audio );
    if( vlc_clone( &p_thread->thread,
                   Thread, p_thread, VLC_THREAD_PRIORITY_LOW ) )
    {
        msg_Err( p_filter, "cannot lauch goom thread" );
        vlc_mutex_destroy( &p_thread->lock );
        vlc_cond_destroy( &p_thread->wait );
        aout_filter_RequestVout( p_filter, p_thread->p_vout, NULL );
        free( p_thread );
        free( p_sys );
        return VLC_EGENERIC;
    }
    p_filter->fmt_in.audio.i_format = VLC_CODEC_FL32;
    p_filter->fmt_out.audio = p_filter->fmt_in.audio;
    p_filter->pf_audio_filter = DoWork;
    return VLC_SUCCESS;
}
