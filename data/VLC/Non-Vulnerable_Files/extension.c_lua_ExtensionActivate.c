}
int lua_ExtensionActivate( extensions_manager_t *p_mgr, extension_t *p_ext )
{
    assert( p_mgr != NULL && p_ext != NULL );
    return lua_ExecuteFunction( p_mgr, p_ext, "activate", LUA_END );
}
