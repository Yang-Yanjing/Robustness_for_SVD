}
static void VariablesInit( playlist_t *p_playlist )
{
    /* These variables control updates */
    var_Create( p_playlist, "intf-change", VLC_VAR_BOOL );
    var_SetBool( p_playlist, "intf-change", true );
    var_Create( p_playlist, "item-change", VLC_VAR_ADDRESS );
    var_Create( p_playlist, "leaf-to-parent", VLC_VAR_INTEGER );
    var_Create( p_playlist, "playlist-item-deleted", VLC_VAR_INTEGER );
    var_SetInteger( p_playlist, "playlist-item-deleted", -1 );
    var_Create( p_playlist, "playlist-item-append", VLC_VAR_ADDRESS );
    var_Create( p_playlist, "input-current", VLC_VAR_ADDRESS );
    var_Create( p_playlist, "activity", VLC_VAR_VOID );
    /* Variables to control playback */
    var_Create( p_playlist, "playlist-autostart", VLC_VAR_BOOL | VLC_VAR_DOINHERIT );
    var_Create( p_playlist, "random", VLC_VAR_BOOL | VLC_VAR_DOINHERIT );
    var_AddCallback( p_playlist, "random", RandomCallback, NULL );
    var_Create( p_playlist, "repeat", VLC_VAR_BOOL | VLC_VAR_DOINHERIT );
    var_Create( p_playlist, "loop", VLC_VAR_BOOL | VLC_VAR_DOINHERIT );
    var_Create( p_playlist, "corks", VLC_VAR_INTEGER );
    var_AddCallback( p_playlist, "corks", CorksCallback, NULL );
    var_Create( p_playlist, "rate", VLC_VAR_FLOAT | VLC_VAR_DOINHERIT );
    var_AddCallback( p_playlist, "rate", RateCallback, NULL );
    var_Create( p_playlist, "rate-slower", VLC_VAR_VOID );
    var_AddCallback( p_playlist, "rate-slower", RateOffsetCallback, NULL );
    var_Create( p_playlist, "rate-faster", VLC_VAR_VOID );
    var_AddCallback( p_playlist, "rate-faster", RateOffsetCallback, NULL );
    var_Create( p_playlist, "video-splitter", VLC_VAR_STRING | VLC_VAR_DOINHERIT );
    var_AddCallback( p_playlist, "video-splitter", VideoSplitterCallback, NULL );
    /* */
    var_Create( p_playlist, "album-art", VLC_VAR_INTEGER | VLC_VAR_DOINHERIT );
    var_Create( p_playlist, "metadata-network-access", VLC_VAR_BOOL | VLC_VAR_DOINHERIT );
    /* Variables to preserve video output parameters */
    var_Create( p_playlist, "fullscreen", VLC_VAR_BOOL | VLC_VAR_DOINHERIT );
    var_Create( p_playlist, "video-on-top", VLC_VAR_BOOL | VLC_VAR_DOINHERIT );
    var_Create( p_playlist, "video-wallpaper", VLC_VAR_BOOL | VLC_VAR_DOINHERIT );
    /* Audio output parameters */
    var_Create( p_playlist, "mute", VLC_VAR_BOOL );
    var_Create( p_playlist, "volume", VLC_VAR_FLOAT );
    var_SetFloat( p_playlist, "volume", -1.f );
}
