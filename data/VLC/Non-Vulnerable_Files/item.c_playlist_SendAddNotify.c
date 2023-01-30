 */
void playlist_SendAddNotify( playlist_t *p_playlist, int i_item_id,
                             int i_node_id, bool b_signal )
{
    playlist_private_t *p_sys = pl_priv(p_playlist);
    PL_ASSERT_LOCKED;
    p_sys->b_reset_currently_playing = true;
    if( b_signal )
        vlc_cond_signal( &p_sys->signal );
    playlist_add_t add;
    add.i_item = i_item_id;
    add.i_node = i_node_id;
    var_SetAddress( p_playlist, "playlist-item-append", &add );
}
