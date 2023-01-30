static void VariablesInit( playlist_t *p_playlist );
static int RandomCallback( vlc_object_t *p_this, char const *psz_cmd,
                           vlc_value_t oldval, vlc_value_t newval, void *a )
{
    (void)psz_cmd; (void)oldval; (void)newval; (void)a;
    playlist_t *p_playlist = (playlist_t*)p_this;
    bool random = newval.b_bool;
    PL_LOCK;
    if( !random ) {
        pl_priv(p_playlist)->b_reset_currently_playing = true;
        vlc_cond_signal( &pl_priv(p_playlist)->signal );
    } else {
        /* Shuffle and sync the playlist on activation of random mode.
         * This preserves the current playing item, so that the user
         * can return to it if needed. (See #4472)
         */
        playlist_private_t *p_sys = pl_priv(p_playlist);
        playlist_item_t *p_new = p_sys->status.p_item;
        ResetCurrentlyPlaying( p_playlist, NULL );
        if( p_new )
            ResyncCurrentIndex( p_playlist, p_new );
    }
    PL_UNLOCK;
    return VLC_SUCCESS;
}
