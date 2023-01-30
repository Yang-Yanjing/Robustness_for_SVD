}
int lua_ExtensionWidgetClick( extensions_manager_t *p_mgr,
                              extension_t *p_ext,
                              extension_widget_t *p_widget )
{
    if( !p_ext->p_sys->L )
        return VLC_SUCCESS;
    lua_State *L = GetLuaState( p_mgr, p_ext );
    lua_pushlightuserdata( L, p_widget );
    lua_gettable( L, LUA_REGISTRYINDEX );
    return lua_ExecuteFunction( p_mgr, p_ext, NULL, LUA_END );
}
