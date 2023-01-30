 */
int playlist_NodeAddCopy (
    playlist_t *p_playlist, playlist_item_t *p_item,
    playlist_item_t *p_parent, int i_pos )
{
    PL_ASSERT_LOCKED;
    assert( p_parent != NULL && p_item != NULL );
    assert( p_parent->i_children > -1 );
    if( i_pos == PLAYLIST_END ) i_pos = p_parent->i_children;
    bool b_flat = false;
    playlist_item_t *p_up = p_parent;
    while( p_up )
    {
        if( p_up == p_playlist->p_playing )
            if( !pl_priv(p_playlist)->b_tree ) b_flat = true;
        if( p_up == p_item )
            /* TODO: We don't support copying a node into itself (yet),
            because we insert items as we copy. Instead, we should copy
            all items first and then insert. */
            return i_pos;
        p_up = p_up->p_parent;
    }
    return RecursiveInsertCopy( p_playlist, p_item, p_parent, i_pos, b_flat );
}
