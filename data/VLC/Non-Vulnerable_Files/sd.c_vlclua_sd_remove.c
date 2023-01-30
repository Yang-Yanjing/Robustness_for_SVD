}
static int vlclua_sd_remove( lua_State *L )
{
    const char *psz_sd = luaL_checkstring( L, 1 );
    playlist_t *p_playlist = vlclua_get_playlist_internal( L );
    int i_ret = playlist_ServicesDiscoveryRemove( p_playlist, psz_sd );
    return vlclua_push_ret( L, i_ret );
}
