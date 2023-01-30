}
void KillExtension( extensions_manager_t *p_mgr, extension_t *p_ext )
{
    /* Cancel thread if it seems stuck for a while */
    msg_Dbg( p_mgr, "Killing extension now" );
    vlc_cancel( p_ext->p_sys->thread );
    lua_ExtensionDeactivate( p_mgr, p_ext );
    p_ext->p_sys->b_exiting = true;
    RemoveActivated( p_mgr, p_ext );
}
