}
static int vlclua_sd_is_loaded( lua_State *L )
{
    const char *psz_sd = luaL_checkstring( L, 1 );
    playlist_t *p_playlist = vlclua_get_playlist_internal( L );
    lua_pushboolean( L, playlist_IsServicesDiscoveryLoaded( p_playlist, psz_sd ));
    return 1;
}
