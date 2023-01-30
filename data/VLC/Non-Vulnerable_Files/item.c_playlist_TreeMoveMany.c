 */
int playlist_TreeMoveMany( playlist_t *p_playlist,
                            int i_items, playlist_item_t **pp_items,
                            playlist_item_t *p_node, int i_newpos )
{
    PL_ASSERT_LOCKED;
    if ( p_node->i_children == -1 ) return VLC_EGENERIC;
    int i;
    for( i = 0; i < i_items; i++ )
    {
        playlist_item_t *p_item = pp_items[i];
        int i_index = ItemIndex( p_item );
        playlist_item_t *p_parent = p_item->p_parent;
        REMOVE_ELEM( p_parent->pp_children, p_parent->i_children, i_index );
        if ( p_parent == p_node && i_index < i_newpos ) i_newpos--;
    }
    for( i = i_items - 1; i >= 0; i-- )
    {
        playlist_item_t *p_item = pp_items[i];
        INSERT_ELEM( p_node->pp_children, p_node->i_children, i_newpos, p_item );
        p_item->p_parent = p_node;
    }
    pl_priv( p_playlist )->b_reset_currently_playing = true;
    vlc_cond_signal( &pl_priv( p_playlist )->signal );
    return VLC_SUCCESS;
}
