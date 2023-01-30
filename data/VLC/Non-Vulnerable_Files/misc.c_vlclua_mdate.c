}
static int vlclua_mdate( lua_State *L )
{
    lua_pushnumber( L, mdate() );
    return 1;
}
