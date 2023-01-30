 */
int playlist_DeleteFromItemId( playlist_t *p_playlist, int i_id )
{
    PL_ASSERT_LOCKED;
    playlist_item_t *p_item = playlist_ItemGetById( p_playlist, i_id );
    if( !p_item ) return VLC_EGENERIC;
    return playlist_DeleteItem( p_playlist, p_item, true );
}
