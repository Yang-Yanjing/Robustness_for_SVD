};
void luaopen_sd_sd( lua_State *L )
{
    lua_newtable( L );
    luaL_register( L, NULL, vlclua_sd_sd_reg );
    lua_setfield( L, -2, "sd" );
}
