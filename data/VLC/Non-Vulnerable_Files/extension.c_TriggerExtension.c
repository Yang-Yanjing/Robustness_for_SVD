 **/
static int TriggerExtension( extensions_manager_t *p_mgr,
                             extension_t *p_ext )
{
    int i_ret = lua_ExecuteFunction( p_mgr, p_ext, "trigger", LUA_END );
    /* Close lua state for trigger-only extensions */
    if( p_ext->p_sys->L )
    {
        vlclua_fd_cleanup( &p_ext->p_sys->dtable );
        lua_close( p_ext->p_sys->L );
    }
    p_ext->p_sys->L = NULL;
    return i_ret;
}
