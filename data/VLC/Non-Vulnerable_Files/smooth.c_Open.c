}
static int Open( vlc_object_t *p_this )
{
    stream_t *s = (stream_t*)p_this;
    stream_sys_t *p_sys;
    if( !isSmoothStreaming( s ) )
        return VLC_EGENERIC;
    msg_Info( p_this, "Smooth Streaming (%s)", s->psz_path );
    s->p_sys = p_sys = calloc( 1, sizeof(*p_sys ) );
    if( unlikely( p_sys == NULL ) )
        return VLC_ENOMEM;
    char *uri = NULL;
    if( unlikely( asprintf( &uri, "%s://%s", s->psz_access, s->psz_path ) < 0 ) )
    {
        free( p_sys );
        return VLC_ENOMEM;
    }
    /* remove the last part of the url */
    char *pos = strrchr( uri, '/');
    *pos = '\0';
    p_sys->base_url = uri;
    /* XXX I don't know wether or not we should allow caching */
    p_sys->b_cache = false;
    p_sys->sms_streams = vlc_array_new();
    p_sys->selected_st = vlc_array_new();
    p_sys->download.chunks = vlc_array_new();
    p_sys->init_chunks = vlc_array_new();
    if( unlikely( !p_sys->sms_streams || !p_sys->download.chunks ||
                  !p_sys->selected_st || !p_sys->init_chunks ) )
    {
        SysCleanup( p_sys );
        free( p_sys );
        return VLC_ENOMEM;
    }
    /* Parse SMS ismc content. */
    if( parse_Manifest( s ) != VLC_SUCCESS )
    {
        SysCleanup( p_sys );
        free( p_sys );
        return VLC_EGENERIC;
    }
    if( !p_sys->vod_duration )
       p_sys->b_live = true;
    p_sys->i_tracks = vlc_array_count( p_sys->sms_streams );
    /* Choose first video / audio / subtitle stream available */
    sms_stream_t *tmp = NULL, *selected = NULL;
    for( unsigned i = 0; i < p_sys->i_tracks; i++ )
    {
        tmp = vlc_array_item_at_index( p_sys->sms_streams, i );
        selected = SMS_GET_SELECTED_ST( tmp->type );
        if( !selected )
            vlc_array_append( p_sys->selected_st, tmp );
    }
    /* Choose lowest quality for the first chunks */
    quality_level_t *wanted, *qlvl;
    sms_stream_t *sms = NULL;
    for( unsigned i = 0; i < p_sys->i_tracks; i++ )
    {
        wanted = qlvl = NULL;
        sms = vlc_array_item_at_index( p_sys->sms_streams, i );
        if ( vlc_array_count( sms->qlevels ) )
        {
            wanted = vlc_array_item_at_index( sms->qlevels, 0 );
            for( unsigned i=1; i < sms->qlevel_nb; i++ )
            {
                qlvl = vlc_array_item_at_index( sms->qlevels, i );
                if( qlvl->Bitrate < wanted->Bitrate )
                    wanted = qlvl;
            }
            sms->download_qlvl = wanted->id;
        }
    }
    vlc_mutex_init( &p_sys->download.lock_wait );
    vlc_cond_init( &p_sys->download.wait );
    /* */
    s->pf_read = Read;
    s->pf_peek = Peek;
    s->pf_control = Control;
    if( vlc_clone( &p_sys->thread, sms_Thread, s, VLC_THREAD_PRIORITY_INPUT ) )
    {
        SysCleanup( p_sys );
        vlc_mutex_destroy( &p_sys->download.lock_wait );
        vlc_cond_destroy( &p_sys->download.wait );
        free( p_sys );
        return VLC_EGENERIC;
    }
    return VLC_SUCCESS;
}
