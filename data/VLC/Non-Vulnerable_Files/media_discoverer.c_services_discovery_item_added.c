 **************************************************************************/
static void services_discovery_item_added( const vlc_event_t * p_event,
                                           void * user_data )
{
    input_item_t * p_item = p_event->u.services_discovery_item_added.p_new_item;
    const char * psz_cat = p_event->u.services_discovery_item_added.psz_category;
    libvlc_media_t * p_md;
    libvlc_media_discoverer_t * p_mdis = user_data;
    libvlc_media_list_t * p_mlist = p_mdis->p_mlist;
    p_md = libvlc_media_new_from_input_item( p_mdis->p_libvlc_instance,
                                             p_item );
    /* If we have a category, that mean we have to group the items having
     * that category in a media_list. */
    if( psz_cat )
    {
        p_mlist = vlc_dictionary_value_for_key( &p_mdis->catname_to_submedialist, psz_cat );
        if( p_mlist == kVLCDictionaryNotFound )
        {
            libvlc_media_t * p_catmd;
            p_catmd = libvlc_media_new_as_node( p_mdis->p_libvlc_instance, psz_cat );
            p_mlist = libvlc_media_subitems( p_catmd );
            p_mlist->b_read_only = true;
            /* Insert the newly created mlist in our dictionary */
            vlc_dictionary_insert( &p_mdis->catname_to_submedialist, psz_cat, p_mlist );
            /* Insert the md into the root list */
            libvlc_media_list_lock( p_mdis->p_mlist );
            _libvlc_media_list_add_media( p_mdis->p_mlist, p_catmd );
            libvlc_media_list_unlock( p_mdis->p_mlist );
            /* We don't release the mlist cause the dictionary
             * doesn't retain the object. But we release the md. */
            libvlc_media_release( p_catmd );
        }
    }
    libvlc_media_list_lock( p_mlist );
    _libvlc_media_list_add_media( p_mlist, p_md );
    libvlc_media_list_unlock( p_mlist );
    libvlc_media_release( p_md );
}
