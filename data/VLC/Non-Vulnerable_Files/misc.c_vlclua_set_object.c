 *****************************************************************************/
void vlclua_set_object( lua_State *L, void *id, void *value )
{
    lua_pushlightuserdata( L, id );
    lua_pushlightuserdata( L, value );
    lua_rawset( L, LUA_REGISTRYINDEX );
}
