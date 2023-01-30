void
services_discovery_AddItem ( services_discovery_t * p_sd, input_item_t * p_item,
                             const char * psz_category )
{
    vlc_event_t event;
    event.type = vlc_ServicesDiscoveryItemAdded;
    event.u.services_discovery_item_added.p_new_item = p_item;
    event.u.services_discovery_item_added.psz_category = psz_category;
    vlc_event_send( &p_sd->event_manager, &event );
}
