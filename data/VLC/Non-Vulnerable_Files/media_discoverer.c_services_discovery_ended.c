 **************************************************************************/
static void services_discovery_ended( const vlc_event_t * p_event,
                                      void * user_data )
{
    VLC_UNUSED(p_event);
    libvlc_media_discoverer_t * p_mdis = user_data;
    libvlc_event_t event;
    p_mdis->running = false;
    event.type = libvlc_MediaDiscovererEnded;
    libvlc_event_send( p_mdis->p_event_manager, &event );
}
