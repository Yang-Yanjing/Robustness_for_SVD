 *****************************************************************************/
playlist_item_t *playlist_ItemNewFromInput( playlist_t *p_playlist,
                                              input_item_t *p_input )
{
    playlist_item_t* p_item = malloc( sizeof( playlist_item_t ) );
    if( !p_item )
        return NULL;
    assert( p_input );
    p_item->p_input = p_input;
    vlc_gc_incref( p_item->p_input );
    p_item->i_id = ++pl_priv(p_playlist)->i_last_playlist_id;
    p_item->p_parent = NULL;
    p_item->i_children = -1;
    p_item->pp_children = NULL;
    p_item->i_flags = 0;
    p_item->p_playlist = p_playlist;
    install_input_item_observer( p_item );
    return p_item;
}
