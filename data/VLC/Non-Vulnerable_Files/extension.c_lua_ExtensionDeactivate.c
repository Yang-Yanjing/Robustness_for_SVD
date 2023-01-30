}
int lua_ExtensionDeactivate( extensions_manager_t *p_mgr, extension_t *p_ext )
{
    assert( p_mgr != NULL && p_ext != NULL );
    if( !p_ext->p_sys->L )
        return VLC_SUCCESS;
    vlclua_fd_interrupt( &p_ext->p_sys->dtable );
    // Unset and release input objects
    if( p_ext->p_sys->p_input )
    {
        if( p_ext->p_sys->i_capabilities & EXT_META_LISTENER )
        {
            // Release item
            input_item_t *p_item = input_GetItem( p_ext->p_sys->p_input );
            vlc_gc_decref( p_item );
        }
        vlc_object_release( p_ext->p_sys->p_input );
        p_ext->p_sys->p_input = NULL;
    }
    int i_ret = lua_ExecuteFunction( p_mgr, p_ext, "deactivate", LUA_END );
    /* Clear Lua State */
    vlclua_fd_cleanup( &p_ext->p_sys->dtable );
    lua_close( p_ext->p_sys->L );
    p_ext->p_sys->L = NULL;
    return i_ret;
}
