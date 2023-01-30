 ***********************************************************************/
services_discovery_t *vlc_sd_Create( vlc_object_t *p_super,
                                     const char *cfg )
{
    services_discovery_t *p_sd;
    p_sd = vlc_custom_create( p_super, sizeof( *p_sd ), "services discovery" );
    if( !p_sd )
        return NULL;
    free(config_ChainCreate( &p_sd->psz_name, &p_sd->p_cfg, cfg ));
    vlc_event_manager_t *em = &p_sd->event_manager;
    vlc_event_manager_init( em, p_sd );
    vlc_event_manager_register_event_type(em, vlc_ServicesDiscoveryItemAdded);
    vlc_event_manager_register_event_type(em, vlc_ServicesDiscoveryItemRemoved);
    vlc_event_manager_register_event_type(em, vlc_ServicesDiscoveryItemRemoveAll);
    vlc_event_manager_register_event_type(em, vlc_ServicesDiscoveryStarted);
    vlc_event_manager_register_event_type(em, vlc_ServicesDiscoveryEnded);
    vlc_object_set_destructor( p_sd, services_discovery_Destructor );
    return p_sd;
}
