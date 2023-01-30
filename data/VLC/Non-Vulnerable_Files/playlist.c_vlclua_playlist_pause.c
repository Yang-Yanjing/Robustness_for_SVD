}
static int vlclua_playlist_pause( lua_State * L )
{
    playlist_t *p_playlist = vlclua_get_playlist_internal( L );
    playlist_Pause( p_playlist );
    return 0;
}
