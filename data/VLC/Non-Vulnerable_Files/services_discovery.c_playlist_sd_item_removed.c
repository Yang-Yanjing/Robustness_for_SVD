 /* A new item has been removed from a certain sd */
static void playlist_sd_item_removed( const vlc_event_t * p_event, void * user_data )
{
    input_item_t * p_input = p_event->u.services_discovery_item_removed.p_item;
    playlist_item_t * p_sd_node = user_data;
    playlist_t *p_playlist = p_sd_node->p_playlist;
    PL_LOCK;
    playlist_item_t *p_item =
        playlist_ItemFindFromInputAndRoot( p_playlist, p_input,
                                           p_sd_node, false );
    if( !p_item )
    {
        PL_UNLOCK; return;
    }
    playlist_item_t *p_parent = p_item->p_parent;
    /* if the item was added under a category and the category node
       becomes empty, delete that node as well */
    if( p_parent->i_children > 1 || p_parent == p_sd_node )
        playlist_DeleteItem( p_playlist, p_item, true );
    else
        playlist_NodeDelete( p_playlist, p_parent, true, true );
    PL_UNLOCK;
}
