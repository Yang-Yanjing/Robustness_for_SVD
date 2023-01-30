 **************************************************************************/
static void services_discovery_item_removed( const vlc_event_t * p_event,
                                             void * user_data )
{
    input_item_t * p_item = p_event->u.services_discovery_item_added.p_new_item;
    libvlc_media_t * p_md;
    libvlc_media_discoverer_t * p_mdis = user_data;
    int i, count = libvlc_media_list_count( p_mdis->p_mlist );
    libvlc_media_list_lock( p_mdis->p_mlist );
    for( i = 0; i < count; i++ )
    {
        p_md = libvlc_media_list_item_at_index( p_mdis->p_mlist, i );
        if( p_md->p_input_item == p_item )
        {
            _libvlc_media_list_remove_index( p_mdis->p_mlist, i );
            break;
        }
    }
    libvlc_media_list_unlock( p_mdis->p_mlist );
}
