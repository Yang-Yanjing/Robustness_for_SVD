}
static int vlclua_playlist_skip( lua_State * L )
{
    int i_skip = luaL_checkint( L, 1 );
    playlist_t *p_playlist = vlclua_get_playlist_internal( L );
    playlist_Skip( p_playlist, i_skip );
    return 0;
}
