/* Do the actual removal */
int playlist_DeleteItem( playlist_t * p_playlist, playlist_item_t *p_item,
                        bool b_stop )
{
    assert( b_stop );
    return playlist_NodeDelete( p_playlist, p_item, true, false );
}
