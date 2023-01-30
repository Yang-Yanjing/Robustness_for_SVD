/* Actually convert an item to a node */
static void ChangeToNode( playlist_t *p_playlist, playlist_item_t *p_item )
{
    int i;
    if( p_item->i_children != -1 ) return;
    p_item->i_children = 0;
    input_item_t *p_input = p_item->p_input;
    vlc_mutex_lock( &p_input->lock );
    p_input->i_type = ITEM_TYPE_NODE;
    vlc_mutex_unlock( &p_input->lock );
    var_SetAddress( p_playlist, "item-change", p_item->p_input );
    /* Remove it from the array of available items */
    ARRAY_BSEARCH( p_playlist->items,->i_id, int, p_item->i_id, i );
    if( i != -1 )
        ARRAY_REMOVE( p_playlist->items, i );
}
