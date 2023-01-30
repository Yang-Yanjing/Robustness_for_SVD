libvlc_media_discoverer_t *
libvlc_media_discoverer_new_from_name( libvlc_instance_t * p_inst,
                                       const char * psz_name )
{
    /* podcast SD is a hack and only works with custom playlist callbacks. */
    if( !strncasecmp( psz_name, "podcast", 7 ) )
        return NULL;
    libvlc_media_discoverer_t *p_mdis = malloc(sizeof(*p_mdis));
    if( unlikely(!p_mdis) )
    {
        libvlc_printerr( "Not enough memory" );
        return NULL;
    }
    p_mdis->p_libvlc_instance = p_inst;
    p_mdis->p_mlist = libvlc_media_list_new( p_inst );
    p_mdis->p_mlist->b_read_only = true;
    p_mdis->running = false;
    vlc_dictionary_init( &p_mdis->catname_to_submedialist, 0 );
    p_mdis->p_event_manager = libvlc_event_manager_new( p_mdis, p_inst );
    if( unlikely(p_mdis->p_event_manager == NULL) )
    {
        free( p_mdis );
        return NULL;
    }
    libvlc_event_manager_register_event_type( p_mdis->p_event_manager,
            libvlc_MediaDiscovererStarted );
    libvlc_event_manager_register_event_type( p_mdis->p_event_manager,
            libvlc_MediaDiscovererEnded );
    p_mdis->p_sd = vlc_sd_Create( (vlc_object_t*)p_inst->p_libvlc_int,
                                  psz_name );
    if( unlikely(p_mdis->p_sd == NULL) )
    {
        libvlc_printerr( "%s: no such discovery module found", psz_name );
        libvlc_media_list_release( p_mdis->p_mlist );
        libvlc_event_manager_release( p_mdis->p_event_manager );
        free( p_mdis );
        return NULL;
    }
    vlc_event_attach( services_discovery_EventManager( p_mdis->p_sd ),
                      vlc_ServicesDiscoveryItemAdded,
                      services_discovery_item_added,
                      p_mdis );
    vlc_event_attach( services_discovery_EventManager( p_mdis->p_sd ),
                      vlc_ServicesDiscoveryItemRemoved,
                      services_discovery_item_removed,
                      p_mdis );
    vlc_event_attach( services_discovery_EventManager( p_mdis->p_sd ),
                      vlc_ServicesDiscoveryStarted,
                      services_discovery_started,
                      p_mdis );
    vlc_event_attach( services_discovery_EventManager( p_mdis->p_sd ),
                      vlc_ServicesDiscoveryEnded,
                      services_discovery_ended,
                      p_mdis );
    vlc_event_attach( services_discovery_EventManager( p_mdis->p_sd ),
                      vlc_ServicesDiscoveryItemRemoveAll,
                      services_discovery_removeall,
                      p_mdis );
    /* Here we go */
    if( !vlc_sd_Start( p_mdis->p_sd ) )
    {
        libvlc_printerr( "%s: internal module error",
                         p_mdis->p_sd->psz_name );
        libvlc_media_list_release( p_mdis->p_mlist );
        libvlc_event_manager_release( p_mdis->p_event_manager );
        free( p_mdis );
        return NULL;
    }
    return p_mdis;
}
