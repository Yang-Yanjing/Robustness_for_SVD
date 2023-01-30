 */
int playlist_TreeMove( playlist_t * p_playlist, playlist_item_t *p_item,
                       playlist_item_t *p_node, int i_newpos )
{
    PL_ASSERT_LOCKED;
    if( p_node->i_children == -1 ) return VLC_EGENERIC;
    playlist_item_t *p_detach = p_item->p_parent;
    int i_index = ItemIndex( p_item );
    REMOVE_ELEM( p_detach->pp_children, p_detach->i_children, i_index );
    if( p_detach == p_node && i_index < i_newpos )
        i_newpos--;
    INSERT_ELEM( p_node->pp_children, p_node->i_children, i_newpos, p_item );
    p_item->p_parent = p_node;
    pl_priv( p_playlist )->b_reset_currently_playing = true;
    vlc_cond_signal( &pl_priv( p_playlist )->signal );
    return VLC_SUCCESS;
}
