 **************************************************************************/
static void services_discovery_started( const vlc_event_t * p_event,
                                        void * user_data )
{
    VLC_UNUSED(p_event);
    libvlc_media_discoverer_t * p_mdis = user_data;
    libvlc_event_t event;
    p_mdis->running = true;
    event.type = libvlc_MediaDiscovererStarted;
    libvlc_event_send( p_mdis->p_event_manager, &event );
}
