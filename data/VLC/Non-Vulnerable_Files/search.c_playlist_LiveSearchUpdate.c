 */
int playlist_LiveSearchUpdate( playlist_t *p_playlist, playlist_item_t *p_root,
                               const char *psz_string, bool b_recursive )
{
    PL_ASSERT_LOCKED;
    pl_priv(p_playlist)->b_reset_currently_playing = true;
    if( *psz_string )
        playlist_LiveSearchUpdateInternal( p_root, psz_string, b_recursive );
    else
        playlist_LiveSearchClean( p_root );
    vlc_cond_signal( &pl_priv(p_playlist)->signal );
    return VLC_SUCCESS;
}
