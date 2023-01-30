 **/
extension_t *vlclua_extension_get( lua_State *L )
{
    lua_pushlightuserdata( L, vlclua_extension_set );
    lua_rawget( L, LUA_REGISTRYINDEX );
    extension_t *p_ext = (extension_t*) lua_topointer( L, -1 );
    lua_pop( L, 1 );
    return p_ext;
}
