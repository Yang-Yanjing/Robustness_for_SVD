 ***********************************************************************/
bool vlc_sd_Start ( services_discovery_t * p_sd )
{
    assert(!p_sd->p_module);
    p_sd->p_module = module_need( p_sd, "services_discovery",
                                  p_sd->psz_name, true );
    if( p_sd->p_module == NULL )
    {
        msg_Err( p_sd, "no suitable services discovery module" );
        return false;
    }
    vlc_event_t event = {
        .type = vlc_ServicesDiscoveryStarted
    };
    vlc_event_send( &p_sd->event_manager, &event );
    return true;
}
