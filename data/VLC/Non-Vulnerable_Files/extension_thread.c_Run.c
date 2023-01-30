/* Thread loop */
static void* Run( void *data )
{
    extension_t *p_ext = data;
    extensions_manager_t *p_mgr = p_ext->p_sys->p_mgr;
    vlc_mutex_lock( &p_ext->p_sys->command_lock );
    mutex_cleanup_push( &p_ext->p_sys->command_lock );
    while( !p_ext->p_sys->b_exiting )
    {
        struct command_t *cmd = p_ext->p_sys->command;
        /* Pop command in front */
        if( cmd == NULL )
        {
            vlc_cond_wait( &p_ext->p_sys->wait, &p_ext->p_sys->command_lock );
            continue;
        }
        p_ext->p_sys->command = cmd->next;
        cmd->next = NULL; /* unlink command (for FreeCommands()) */
        vlc_mutex_unlock( &p_ext->p_sys->command_lock );
        /* Run command */
        int cancel = vlc_savecancel();
        if( LockExtension( p_ext ) )
        {
            switch( cmd->i_command )
            {
                case CMD_ACTIVATE:
                {
                    if( lua_ExecuteFunction( p_mgr, p_ext, "activate", LUA_END ) < 0 )
                    {
                        msg_Err( p_mgr, "Could not activate extension!" );
                        Deactivate( p_mgr, p_ext );
                    }
                    break;
                }
                case CMD_DEACTIVATE:
                {
                    msg_Dbg( p_mgr, "Deactivating '%s'", p_ext->psz_title );
                    if( lua_ExtensionDeactivate( p_mgr, p_ext ) < 0 )
                    {
                        msg_Warn( p_mgr, "Extension '%s' did not deactivate properly",
                                  p_ext->psz_title );
                    }
                    p_ext->p_sys->b_exiting = true;
                    RemoveActivated( p_mgr, p_ext );
                    break;
                }
                case CMD_CLOSE:
                {
                    lua_ExecuteFunction( p_mgr, p_ext, "close", LUA_END );
                    break;
                }
                case CMD_CLICK:
                {
                    extension_widget_t *p_widget = cmd->data[0];
                    assert( p_widget );
                    msg_Dbg( p_mgr, "Clicking '%s': '%s'",
                             p_ext->psz_name, p_widget->psz_text );
                    if( lua_ExtensionWidgetClick( p_mgr, p_ext, p_widget ) < 0 )
                    {
                        msg_Warn( p_mgr, "Could not translate click" );
                    }
                    break;
                }
                case CMD_TRIGGERMENU:
                {
                    int *pi_id = cmd->data[0];
                    assert( pi_id );
                    msg_Dbg( p_mgr, "Trigger menu %d of '%s'",
                             *pi_id, p_ext->psz_name );
                    lua_ExtensionTriggerMenu( p_mgr, p_ext, *pi_id );
                    break;
                }
                case CMD_SET_INPUT:
                {
                    lua_ExecuteFunction( p_mgr, p_ext, "input_changed", LUA_END );
                    break;
                }
                case CMD_UPDATE_META:
                {
                    lua_ExecuteFunction( p_mgr, p_ext, "meta_changed", LUA_END );
                    break;
                }
                case CMD_PLAYING_CHANGED:
                {
                    lua_ExecuteFunction( p_mgr, p_ext, "playing_changed",
                            LUA_NUM, *((int *)cmd->data[0]), LUA_END );
                    break;
                }
                default:
                {
                    msg_Dbg( p_mgr, "Unknown command in extension command queue: %d",
                             cmd->i_command );
                    break;
                }
            }
            UnlockExtension( p_ext );
        }
        FreeCommands( cmd );
        vlc_restorecancel( cancel );
        vlc_mutex_lock( &p_ext->p_sys->command_lock );
    }
    vlc_cleanup_run( );
    msg_Dbg( p_mgr, "Extension thread end: '%s'", p_ext->psz_title );
    // Note: At this point, the extension should be deactivated
    return NULL;
}
