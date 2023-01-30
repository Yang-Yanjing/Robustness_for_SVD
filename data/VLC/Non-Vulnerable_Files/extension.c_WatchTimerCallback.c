 **/
static void WatchTimerCallback( void *data )
{
    extension_t *p_ext = data;
    extensions_manager_t *p_mgr = p_ext->p_sys->p_mgr;
    char *message;
    if( asprintf( &message, _( "Extension '%s' does not respond.\n"
                               "Do you want to kill it now? " ),
                  p_ext->psz_title ) == -1 )
    {
        return;
    }
    vlc_mutex_lock( &p_ext->p_sys->command_lock );
    for( struct command_t *cmd = p_ext->p_sys->command;
         cmd != NULL;
         cmd = cmd->next )
        if( cmd->i_command == CMD_DEACTIVATE )
        {   /* We have a pending Deactivate command... */
            if( p_ext->p_sys->progress )
            {
                dialog_ProgressDestroy( p_ext->p_sys->progress );
                p_ext->p_sys->progress = NULL;
            }
            vlc_mutex_unlock( &p_ext->p_sys->command_lock );
            KillExtension( p_mgr, p_ext );
            return;
        }
    if( !p_ext->p_sys->progress )
    {
        p_ext->p_sys->progress =
                dialog_ProgressCreate( p_mgr, _( "Extension not responding!" ),
                                       message,
                                       _( "Yes" ) );
        vlc_timer_schedule( p_ext->p_sys->timer, false, 100000, 0 );
    }
    else
    {
        if( dialog_ProgressCancelled( p_ext->p_sys->progress ) )
        {
            dialog_ProgressDestroy( p_ext->p_sys->progress );
            p_ext->p_sys->progress = NULL;
            vlc_mutex_unlock( &p_ext->p_sys->command_lock );
            KillExtension( p_mgr, p_ext );
            return;
        }
        vlc_timer_schedule( p_ext->p_sys->timer, false, 100000, 0 );
    }
    vlc_mutex_unlock( &p_ext->p_sys->command_lock );
}
