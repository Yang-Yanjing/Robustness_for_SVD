 *****************************************************************************/
static void Close( vlc_object_t *p_this )
{
    services_discovery_t *p_sd = ( services_discovery_t* )p_this;
    services_discovery_sys_t *p_sys = p_sd->p_sys;
    avahi_service_browser_free( p_sys->sb );
    avahi_client_free( p_sys->client );
    avahi_threaded_poll_free( p_sys->poll );
    vlc_dictionary_clear( &p_sys->services_name_to_input_item, NULL, NULL );
    free( p_sys );
}
