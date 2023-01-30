};
void luaopen_misc( lua_State *L )
{
    lua_newtable( L );
    luaL_register( L, NULL, vlclua_misc_reg );
    lua_setfield( L, -2, "misc" );
}
