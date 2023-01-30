 **/
void Close_Extension( vlc_object_t *p_this )
{
    extensions_manager_t *p_mgr = ( extensions_manager_t* ) p_this;
    msg_Dbg( p_mgr, "Deactivating all loaded extensions" );
    vlc_mutex_lock( &p_mgr->lock );
    p_mgr->p_sys->b_killed = true;
    vlc_mutex_unlock( &p_mgr->lock );
    var_DelCallback( p_this, "dialog-event",
                     vlclua_extension_dialog_callback, NULL );
    var_Destroy( p_mgr, "dialog-event" );
    extension_t *p_ext = NULL;
    FOREACH_ARRAY( p_ext, p_mgr->p_sys->activated_extensions )
    {
        if( !p_ext ) break;
        msg_Dbg( p_mgr, "Deactivating '%s'", p_ext->psz_title );
        Deactivate( p_mgr, p_ext );
        vlc_join( p_ext->p_sys->thread, NULL );
    }
    FOREACH_END()
    msg_Dbg( p_mgr, "All extensions are now deactivated" );
    ARRAY_RESET( p_mgr->p_sys->activated_extensions );
    vlc_mutex_destroy( &p_mgr->lock );
    vlc_mutex_destroy( &p_mgr->p_sys->lock );
    free( p_mgr->p_sys );
    p_mgr->p_sys = NULL;
    /* Free extensions' memory */
    FOREACH_ARRAY( p_ext, p_mgr->extensions )
    {
        if( !p_ext )
            break;
        if( p_ext->p_sys->L )
            lua_close( p_ext->p_sys->L );
        free( p_ext->psz_name );
        free( p_ext->psz_title );
        free( p_ext->psz_author );
        free( p_ext->psz_description );
        free( p_ext->psz_shortdescription );
        free( p_ext->psz_url );
        free( p_ext->psz_version );
        free( p_ext->p_icondata );
        vlc_mutex_destroy( &p_ext->p_sys->running_lock );
        vlc_mutex_destroy( &p_ext->p_sys->command_lock );
        vlc_cond_destroy( &p_ext->p_sys->wait );
        vlc_timer_destroy( p_ext->p_sys->timer );
        free( p_ext->p_sys );
        free( p_ext );
    }
    FOREACH_END()
    ARRAY_RESET( p_mgr->extensions );
}
