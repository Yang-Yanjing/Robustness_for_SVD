 *****************************************************************************/
static int vlclua_license( lua_State *L )
{
    lua_pushstring( L, LICENSE_MSG );
    return 1;
}
