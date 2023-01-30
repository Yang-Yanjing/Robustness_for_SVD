};
void luaopen_sd_intf( lua_State *L )
{
    lua_newtable( L );
    luaL_register( L, NULL, vlclua_sd_intf_reg );
    lua_setfield( L, -2, "sd" );
}
