}
static int vlclua_playlist_search( lua_State *L )
{
    playlist_t *p_playlist = vlclua_get_playlist_internal( L );
    const char *psz_string = luaL_optstring( L, 1, "" );
    PL_LOCK;
    playlist_LiveSearchUpdate( p_playlist, p_playlist->p_root, psz_string, true );
    PL_UNLOCK;
    push_playlist_item( L, p_playlist->p_root );
    return 1;
}
