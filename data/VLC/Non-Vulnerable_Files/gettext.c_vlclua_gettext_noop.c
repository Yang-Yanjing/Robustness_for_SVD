}
static int vlclua_gettext_noop( lua_State *L )
{
    lua_settop( L, 1 );
    return 1;
}
