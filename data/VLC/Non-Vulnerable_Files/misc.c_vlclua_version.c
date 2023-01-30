 *****************************************************************************/
static int vlclua_version( lua_State *L )
{
    lua_pushstring( L, VERSION_MESSAGE );
    return 1;
}
