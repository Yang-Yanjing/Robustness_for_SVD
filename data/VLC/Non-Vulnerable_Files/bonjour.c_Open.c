 *****************************************************************************/
static int Open( vlc_object_t *p_this )
{
    services_discovery_t *p_sd = ( services_discovery_t* )p_this;
    services_discovery_sys_t *p_sys;
    int err;
    p_sd->p_sys = p_sys = calloc( 1, sizeof( services_discovery_sys_t ) );
    if( !p_sys )
        return VLC_ENOMEM;
    vlc_dictionary_init( &p_sys->services_name_to_input_item, 1 );
    p_sys->poll = avahi_threaded_poll_new();
    if( p_sys->poll == NULL )
    {
        msg_Err( p_sd, "failed to create Avahi threaded poll" );
        goto error;
    }
    p_sys->client = avahi_client_new( avahi_threaded_poll_get(p_sys->poll),
                                      0, client_callback, p_sd, &err );
    if( p_sys->client == NULL )
    {
        msg_Err( p_sd, "failed to create avahi client: %s",
                 avahi_strerror( err ) );
        goto error;
    }
    p_sys->sb = avahi_service_browser_new( p_sys->client, AVAHI_IF_UNSPEC,
                                           AVAHI_PROTO_UNSPEC,
                                           "_vlc-http._tcp", NULL,
                                           0, browse_callback, p_sd );
    if( p_sys->sb == NULL )
    {
        msg_Err( p_sd, "failed to create avahi service browser" );
        goto error;
    }
    return VLC_SUCCESS;
error:
    if( p_sys->sb != NULL )
        avahi_service_browser_free( p_sys->sb );
    if( p_sys->client != NULL )
        avahi_client_free( p_sys->client );
    if( p_sys->poll != NULL )
        avahi_threaded_poll_free( p_sys->poll );
    vlc_dictionary_clear( &p_sys->services_name_to_input_item, NULL, NULL );
    free( p_sys );
    return VLC_EGENERIC;
}
