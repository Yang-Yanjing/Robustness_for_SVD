};
void luaopen_gettext( lua_State *L )
{
    lua_newtable( L );
    luaL_register( L, NULL, vlclua_gettext_reg );
    lua_setfield( L, -2, "gettext" );
}
