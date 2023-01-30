};
void luaopen_config( lua_State *L )
{
    lua_newtable( L );
    luaL_register( L, NULL, vlclua_config_reg );
    lua_setfield( L, -2, "config" );
}
