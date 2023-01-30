}
playlist_t *vlclua_get_playlist_internal( lua_State *L )
{
    return vlclua_get_object( L, vlclua_set_playlist_internal );
}
