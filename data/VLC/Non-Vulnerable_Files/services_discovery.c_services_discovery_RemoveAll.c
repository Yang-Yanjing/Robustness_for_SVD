void
services_discovery_RemoveAll ( services_discovery_t * p_sd )
{
    vlc_event_t event;
    event.type = vlc_ServicesDiscoveryItemRemoveAll;
    vlc_event_send( &p_sd->event_manager, &event );
}
