 **/
int Open_Extension( vlc_object_t *p_this )
{
    msg_Dbg( p_this, "Opening Lua Extension module" );
    extensions_manager_t *p_mgr = ( extensions_manager_t* ) p_this;
    p_mgr->pf_control = Control;
    extensions_manager_sys_t *p_sys = ( extensions_manager_sys_t* )
                    calloc( 1, sizeof( extensions_manager_sys_t ) );
    if( !p_sys ) return VLC_ENOMEM;
    p_mgr->p_sys = p_sys;
    ARRAY_INIT( p_sys->activated_extensions );
    ARRAY_INIT( p_mgr->extensions );
    vlc_mutex_init( &p_mgr->lock );
    vlc_mutex_init( &p_mgr->p_sys->lock );
    /* Scan available Lua Extensions */
    if( ScanExtensions( p_mgr ) != VLC_SUCCESS )
    {
        msg_Err( p_mgr, "Can't load extensions modules" );
        return VLC_EGENERIC;
    }
    // Create the dialog-event variable
    var_Create( p_this, "dialog-event", VLC_VAR_ADDRESS );
    var_AddCallback( p_this, "dialog-event",
                     vlclua_extension_dialog_callback, NULL );
    return VLC_SUCCESS;
}
