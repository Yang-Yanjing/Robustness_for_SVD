 *****************************************************************************/
static void client_callback( AvahiClient *c, AvahiClientState state,
                             void * userdata )
{
    services_discovery_t *p_sd = ( services_discovery_t* )userdata;
    services_discovery_sys_t *p_sys = p_sd->p_sys;
    if( state == AVAHI_CLIENT_FAILURE &&
        (avahi_client_errno(c) == AVAHI_ERR_DISCONNECTED) )
    {
        msg_Err( p_sd, "avahi client disconnected" );
        avahi_threaded_poll_quit( p_sys->poll );
    }
}
