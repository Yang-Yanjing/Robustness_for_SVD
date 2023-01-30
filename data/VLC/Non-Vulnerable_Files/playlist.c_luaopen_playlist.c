};
void luaopen_playlist( lua_State *L )
{
    lua_newtable( L );
    luaL_register( L, NULL, vlclua_playlist_reg );
    lua_setfield( L, -2, "playlist" );
}
