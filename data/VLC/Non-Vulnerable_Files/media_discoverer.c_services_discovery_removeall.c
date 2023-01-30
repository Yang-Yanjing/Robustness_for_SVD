 **************************************************************************/
static void services_discovery_removeall( const vlc_event_t * p_event,
                                             void * user_data )
{
    VLC_UNUSED(p_event);
    libvlc_media_discoverer_t * p_mdis = user_data;
    libvlc_media_list_lock( p_mdis->p_mlist );
    for( int i = 0; i < libvlc_media_list_count( p_mdis->p_mlist ); i++ )
    {
        _libvlc_media_list_remove_index( p_mdis->p_mlist, i );
    }
    libvlc_media_list_unlock( p_mdis->p_mlist );
}
