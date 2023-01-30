 */
void vlclua_extension_set( lua_State *L, extension_t *p_ext )
{
    lua_pushlightuserdata( L, vlclua_extension_set );
    lua_pushlightuserdata( L, p_ext );
    lua_rawset( L, LUA_REGISTRYINDEX );
}
