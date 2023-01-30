 **/
int Activate( extensions_manager_t *p_mgr, extension_t *p_ext )
{
    assert( p_ext != NULL );
    struct extension_sys_t *p_sys = p_ext->p_sys;
    assert( p_sys != NULL );
    msg_Dbg( p_mgr, "Activating extension '%s'", p_ext->psz_title );
    if( IsActivated( p_mgr, p_ext ) )
    {
        msg_Warn( p_mgr, "Extension is already activated!" );
        return VLC_EGENERIC;
    }
    /* Add this script to the activated extensions list */
    vlc_mutex_lock( &p_mgr->p_sys->lock );
    ARRAY_APPEND( p_mgr->p_sys->activated_extensions, p_ext );
    vlc_mutex_unlock( &p_mgr->p_sys->lock );
    /* Prepare first command */
    p_sys->command = calloc( 1, sizeof( struct command_t ) );
    if( !p_sys->command )
        return VLC_ENOMEM;
    p_sys->command->i_command = CMD_ACTIVATE; /* No params */
    /* Start thread */
    p_sys->b_exiting = false;
    if( vlc_clone( &p_sys->thread, Run, p_ext, VLC_THREAD_PRIORITY_LOW )
        != VLC_SUCCESS )
    {
        p_sys->b_exiting = true;
        // Note: Automatically deactivating the extension...
        Deactivate( p_mgr, p_ext );
        return VLC_ENOMEM;
    }
    return VLC_SUCCESS;
}
