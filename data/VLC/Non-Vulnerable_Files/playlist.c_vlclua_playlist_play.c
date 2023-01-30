}
static int vlclua_playlist_play( lua_State * L )
{
    playlist_t *p_playlist = vlclua_get_playlist_internal( L );
    playlist_Play( p_playlist );
    return 0;
}
