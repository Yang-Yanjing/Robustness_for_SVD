/** Deactivate this extension: pushes immediate command and drops queued */
int Deactivate( extensions_manager_t *p_mgr, extension_t *p_ext )
{
    (void) p_mgr;
    vlc_mutex_lock( &p_ext->p_sys->command_lock );
    if( p_ext->p_sys->b_exiting )
    {
        vlc_mutex_unlock( &p_ext->p_sys->command_lock );
        return VLC_EGENERIC;
    }
    if( p_ext->p_sys->progress )
    {
        // Extension is stuck, kill it now
        dialog_ProgressDestroy( p_ext->p_sys->progress );
        p_ext->p_sys->progress = NULL;
        vlc_mutex_unlock( &p_ext->p_sys->command_lock );
        KillExtension( p_mgr, p_ext );
        return VLC_SUCCESS;
    }
    /* Free the list of commands */
    if( p_ext->p_sys->command )
        FreeCommands( p_ext->p_sys->command->next );
    /* Push command */
    struct command_t *cmd = calloc( 1, sizeof( struct command_t ) );
    cmd->i_command = CMD_DEACTIVATE;
    if( p_ext->p_sys->command )
        p_ext->p_sys->command->next = cmd;
    else
        p_ext->p_sys->command = cmd;
    vlc_cond_signal( &p_ext->p_sys->wait );
    vlc_mutex_unlock( &p_ext->p_sys->command_lock );
    return VLC_SUCCESS;
}
