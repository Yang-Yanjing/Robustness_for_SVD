 ***********************************************************************/
void vlc_sd_Stop ( services_discovery_t * p_sd )
{
    vlc_event_t event = {
        .type = vlc_ServicesDiscoveryEnded
    };
    vlc_event_send( &p_sd->event_manager, &event );
    module_unneed( p_sd, p_sd->p_module );
    p_sd->p_module = NULL;
}
