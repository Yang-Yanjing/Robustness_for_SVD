*/
static int DeleteFromInput( playlist_t *p_playlist, input_item_t *p_input,
                            playlist_item_t *p_root, bool b_do_stop )
{
    PL_ASSERT_LOCKED;
    playlist_item_t *p_item = playlist_ItemFindFromInputAndRoot(
        p_playlist, p_input, p_root, false );
    if( !p_item ) return VLC_EGENERIC;
    return playlist_DeleteItem( p_playlist, p_item, b_do_stop );
}
