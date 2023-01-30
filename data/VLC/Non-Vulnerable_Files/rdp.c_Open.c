 *****************************************************************************/
static int Open( vlc_object_t *p_this )
{
    demux_t      *p_demux = (demux_t*)p_this;
    demux_sys_t  *p_sys;
    p_sys = calloc( 1, sizeof(demux_sys_t) );
    if( !p_sys ) return VLC_ENOMEM;
    p_sys->f_fps = var_InheritFloat( p_demux, CFG_PREFIX "fps" );
    if ( p_sys->f_fps <= 0 ) p_sys->f_fps = 1.0;
    p_sys->i_frame_interval = 1000000 / p_sys->f_fps;
#if FREERDP_VERSION_MAJOR == 1 && FREERDP_VERSION_MINOR < 2
    freerdp_channels_global_init();
#endif
    p_sys->p_instance = freerdp_new();
    if ( !p_sys->p_instance )
    {
        msg_Err( p_demux, "rdp instanciation error" );
        free( p_sys );
        return VLC_EGENERIC;
    }
    p_demux->p_sys = p_sys;
    p_sys->p_instance->PreConnect = preConnectHandler;
    p_sys->p_instance->PostConnect = postConnectHandler;
    p_sys->p_instance->Authenticate = authenticateHandler;
    /* Set up context handlers and let it be allocated */
    p_sys->p_instance->ContextSize = sizeof( vlcrdp_context_t );
    freerdp_context_new( p_sys->p_instance );
    vlcrdp_context_t * p_vlccontext = (vlcrdp_context_t *) p_sys->p_instance->context;
    p_vlccontext->p_demux = p_demux;
    /* Parse uri params for pre-connect */
    vlc_url_t url;
    vlc_UrlParse( &url, p_demux->psz_location, 0 );
    if ( !EMPTY_STR(url.psz_host) )
        p_sys->psz_hostname = strdup( url.psz_host );
    else
        p_sys->psz_hostname = strdup( "localhost" );
    p_sys->i_port = ( url.i_port > 0 ) ? url.i_port : 3389;
    vlc_UrlClean( &url );
    if ( ! freerdp_connect( p_sys->p_instance ) )
    {
        msg_Err( p_demux, "can't connect to rdp server" );
        goto error;
    }
    if ( vlc_clone( &p_sys->thread, DemuxThread, p_demux, VLC_THREAD_PRIORITY_INPUT ) != VLC_SUCCESS )
    {
        msg_Err( p_demux, "can't spawn thread" );
        freerdp_disconnect( p_sys->p_instance );
        goto error;
    }
    p_demux->pf_demux = NULL;
    p_demux->pf_control = Control;
    return VLC_SUCCESS;
error:
    freerdp_free( p_sys->p_instance );
    free( p_sys->psz_hostname );
    free( p_sys );
    return VLC_EGENERIC;
}
