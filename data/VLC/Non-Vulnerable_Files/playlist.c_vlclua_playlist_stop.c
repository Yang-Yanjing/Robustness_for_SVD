}
static int vlclua_playlist_stop( lua_State * L )
{
    playlist_t *p_playlist = vlclua_get_playlist_internal( L );
    playlist_Stop( p_playlist );
    return 0;
}
