 */
playlist_item_t * playlist_NodeAddInput( playlist_t *p_playlist,
                                         input_item_t *p_input,
                                         playlist_item_t *p_parent,
                                         int i_mode, int i_pos,
                                         bool b_locked )
{
    playlist_item_t *p_item;
    assert( p_input );
    assert( p_parent && p_parent->i_children != -1 );
    PL_LOCK_IF( !b_locked );
    p_item = playlist_ItemNewFromInput( p_playlist, p_input );
    if( p_item == NULL )
        goto end;
    AddItem( p_playlist, p_item, p_parent, i_mode, i_pos );
    GoAndPreparse( p_playlist, i_mode, p_item );
end:
    PL_UNLOCK_IF( !b_locked );
    return p_item;
}
