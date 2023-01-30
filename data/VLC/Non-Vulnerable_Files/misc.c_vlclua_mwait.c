}
static int vlclua_mwait( lua_State *L )
{
    double f = luaL_checknumber( L, 1 );
    mwait( (int64_t)f );
    return 0;
}
