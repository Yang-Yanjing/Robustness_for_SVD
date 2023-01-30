void
libvlc_media_discoverer_release( libvlc_media_discoverer_t * p_mdis )
{
    int i;
    vlc_event_detach( services_discovery_EventManager( p_mdis->p_sd ),
                     vlc_ServicesDiscoveryItemAdded,
                     services_discovery_item_added,
                     p_mdis );
    vlc_event_detach( services_discovery_EventManager( p_mdis->p_sd ),
                     vlc_ServicesDiscoveryItemRemoved,
                     services_discovery_item_removed,
                     p_mdis );
    vlc_event_detach( services_discovery_EventManager( p_mdis->p_sd ),
                     vlc_ServicesDiscoveryStarted,
                     services_discovery_started,
                     p_mdis );
    vlc_event_detach( services_discovery_EventManager( p_mdis->p_sd ),
                     vlc_ServicesDiscoveryEnded,
                     services_discovery_ended,
                     p_mdis );
    vlc_event_detach( services_discovery_EventManager( p_mdis->p_sd ),
                     vlc_ServicesDiscoveryItemRemoveAll,
                     services_discovery_removeall,
                     p_mdis );
    libvlc_media_list_release( p_mdis->p_mlist );
    vlc_sd_StopAndDestroy( p_mdis->p_sd );
    /* Free catname_to_submedialist and all the mlist */
    char ** all_keys = vlc_dictionary_all_keys( &p_mdis->catname_to_submedialist );
    for( i = 0; all_keys[i]; i++ )
    {
        libvlc_media_list_t * p_catmlist = vlc_dictionary_value_for_key( &p_mdis->catname_to_submedialist, all_keys[i] );
        libvlc_media_list_release( p_catmlist );
        free( all_keys[i] );
    }
    free( all_keys );
    vlc_dictionary_clear( &p_mdis->catname_to_submedialist, NULL, NULL );
    libvlc_event_manager_release( p_mdis->p_event_manager );
    free( p_mdis );
}
