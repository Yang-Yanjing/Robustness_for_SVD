 *****************************************************************************/
static int vlclua_gettext( lua_State *L )
{
    lua_pushstring( L, _( luaL_checkstring( L, 1 ) ) );
    return 1;
}
