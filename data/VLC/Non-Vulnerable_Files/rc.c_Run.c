 *****************************************************************************/
static void *Run( void *data )
{
    intf_thread_t *p_intf = data;
    intf_sys_t *p_sys = p_intf->p_sys;
    char p_buffer[ MAX_LINE_LENGTH + 1 ];
    bool b_showpos = var_InheritBool( p_intf, "rc-show-pos" );
    int  i_size = 0;
    int  i_oldpos = 0;
    int  i_newpos;
    int  canc = vlc_savecancel( );
    p_buffer[0] = 0;
#ifdef _WIN32
    /* Get the file descriptor of the console input */
    p_intf->p_sys->hConsoleIn = GetStdHandle(STD_INPUT_HANDLE);
    if( p_intf->p_sys->hConsoleIn == INVALID_HANDLE_VALUE )
    {
        msg_Err( p_intf, "couldn't find user input handle" );
        return;
    }
#endif
    /* Register commands that will be cleaned up upon object destruction */
    RegisterCallbacks( p_intf );
    /* status callbacks */
    for( ;; )
    {
        char *psz_cmd, *psz_arg;
        bool b_complete;
        vlc_restorecancel( canc );
        if( p_sys->pi_socket_listen != NULL && p_sys->i_socket == -1 )
        {
            p_sys->i_socket =
                net_Accept( p_intf, p_sys->pi_socket_listen );
            if( p_sys->i_socket == -1 ) continue;
        }
        b_complete = ReadCommand( p_intf, p_buffer, &i_size );
        canc = vlc_savecancel( );
        /* Manage the input part */
        if( p_sys->p_input == NULL )
        {
            p_sys->p_input = playlist_CurrentInput( p_sys->p_playlist );
            /* New input has been registered */
            if( p_sys->p_input )
            {
                char *psz_uri = input_item_GetURI( input_GetItem( p_sys->p_input ) );
                msg_rc( STATUS_CHANGE "( new input: %s )", psz_uri );
                free( psz_uri );
                var_AddCallback( p_sys->p_input, "intf-event", InputEvent, p_intf );
            }
        }
#warning This is not reliable...
        else if( p_sys->p_input->b_dead )
        {
            var_DelCallback( p_sys->p_input, "intf-event", InputEvent, p_intf );
            vlc_object_release( p_sys->p_input );
            p_sys->p_input = NULL;
            p_sys->i_last_state = PLAYLIST_STOPPED;
            msg_rc( STATUS_CHANGE "( stop state: 0 )" );
        }
        if( p_sys->p_input != NULL )
        {
            playlist_t *p_playlist = p_sys->p_playlist;
            PL_LOCK;
            int status = playlist_Status( p_playlist );
            PL_UNLOCK;
            if( p_sys->i_last_state != status )
            {
                if( status == PLAYLIST_STOPPED )
                {
                    p_sys->i_last_state = PLAYLIST_STOPPED;
                    msg_rc( STATUS_CHANGE "( stop state: 5 )" );
                }
                else if( status == PLAYLIST_RUNNING )
                {
                    p_sys->i_last_state = PLAYLIST_RUNNING;
                    msg_rc( STATUS_CHANGE "( play state: 3 )" );
                }
                else if( status == PLAYLIST_PAUSED )
                {
                    p_sys->i_last_state = PLAYLIST_PAUSED;
                    msg_rc( STATUS_CHANGE "( pause state: 4 )" );
                }
            }
        }
        if( p_sys->p_input && b_showpos )
        {
            i_newpos = 100 * var_GetFloat( p_sys->p_input, "position" );
            if( i_oldpos != i_newpos )
            {
                i_oldpos = i_newpos;
                msg_rc( "pos: %d%%", i_newpos );
            }
        }
        /* Is there something to do? */
        if( !b_complete ) continue;
        /* Skip heading spaces */
        psz_cmd = p_buffer;
        while( *psz_cmd == ' ' )
        {
            psz_cmd++;
        }
        /* Split psz_cmd at the first space and make sure that
         * psz_arg is valid */
        psz_arg = strchr( psz_cmd, ' ' );
        if( psz_arg )
        {
            *psz_arg++ = 0;
            while( *psz_arg == ' ' )
            {
                psz_arg++;
            }
        }
        else
        {
            psz_arg = (char*)"";
        }
        /* If the user typed a registered local command, try it */
        if( var_Type( p_intf, psz_cmd ) & VLC_VAR_ISCOMMAND )
        {
            vlc_value_t val;
            int i_ret;
            val.psz_string = psz_arg;
            if ((var_Type( p_intf, psz_cmd) & VLC_VAR_CLASS) == VLC_VAR_VOID)
                i_ret = var_TriggerCallback( p_intf, psz_cmd );
            else
                i_ret = var_Set( p_intf, psz_cmd, val );
            msg_rc( "%s: returned %i (%s)",
                    psz_cmd, i_ret, vlc_error( i_ret ) );
        }
        /* Or maybe it's a global command */
        else if( var_Type( p_intf->p_libvlc, psz_cmd ) & VLC_VAR_ISCOMMAND )
        {
            vlc_value_t val;
            int i_ret;
            val.psz_string = psz_arg;
            /* FIXME: it's a global command, but we should pass the
             * local object as an argument, not p_intf->p_libvlc. */
            if ((var_Type( p_intf->p_libvlc, psz_cmd) & VLC_VAR_CLASS) == VLC_VAR_VOID)
                i_ret = var_TriggerCallback( p_intf, psz_cmd );
            else
                i_ret = var_Set( p_intf->p_libvlc, psz_cmd, val );
            if( i_ret != 0 )
            {
                msg_rc( "%s: returned %i (%s)",
                         psz_cmd, i_ret, vlc_error( i_ret ) );
            }
        }
        else if( !strcmp( psz_cmd, "logout" ) )
        {
            /* Close connection */
            if( p_sys->i_socket != -1 )
            {
                net_Close( p_sys->i_socket );
                p_sys->i_socket = -1;
            }
        }
        else if( !strcmp( psz_cmd, "info" ) )
        {
            if( p_sys->p_input )
            {
                int i, j;
                vlc_mutex_lock( &input_GetItem(p_sys->p_input)->lock );
                for ( i = 0; i < input_GetItem(p_sys->p_input)->i_categories; i++ )
                {
                    info_category_t *p_category = input_GetItem(p_sys->p_input)
                                                        ->pp_categories[i];
                    msg_rc( "+----[ %s ]", p_category->psz_name );
                    msg_rc( "| " );
                    for ( j = 0; j < p_category->i_infos; j++ )
                    {
                        info_t *p_info = p_category->pp_infos[j];
                        msg_rc( "| %s: %s", p_info->psz_name,
                                p_info->psz_value );
                    }
                    msg_rc( "| " );
                }
                msg_rc( "+----[ end of stream info ]" );
                vlc_mutex_unlock( &input_GetItem(p_sys->p_input)->lock );
            }
            else
            {
                msg_rc( "no input" );
            }
        }
        else if( !strcmp( psz_cmd, "is_playing" ) )
        {
            if( p_sys->p_input == NULL )
            {
                msg_rc( "0" );
            }
            else
            {
                msg_rc( "1" );
            }
        }
        else if( !strcmp( psz_cmd, "get_time" ) )
        {
            if( p_sys->p_input == NULL )
            {
                msg_rc("0");
            }
            else
            {
                vlc_value_t time;
                var_Get( p_sys->p_input, "time", &time );
                msg_rc( "%"PRIu64, time.i_time / 1000000);
            }
        }
        else if( !strcmp( psz_cmd, "get_length" ) )
        {
            if( p_sys->p_input == NULL )
            {
                msg_rc("0");
            }
            else
            {
                vlc_value_t time;
                var_Get( p_sys->p_input, "length", &time );
                msg_rc( "%"PRIu64, time.i_time / 1000000);
            }
        }
        else if( !strcmp( psz_cmd, "get_title" ) )
        {
            if( p_sys->p_input == NULL )
            {
                msg_rc("%s", "");
            }
            else
            {
                msg_rc( "%s", input_GetItem(p_sys->p_input)->psz_name );
            }
        }
        else if( !strcmp( psz_cmd, "longhelp" ) || !strncmp( psz_cmd, "h", 1 )
                 || !strncmp( psz_cmd, "H", 1 ) || !strncmp( psz_cmd, "?", 1 ) )
        {
            Help( p_intf );
        }
        else if( !strcmp( psz_cmd, "key" ) || !strcmp( psz_cmd, "hotkey" ) )
        {
            var_SetInteger( p_intf->p_libvlc, "key-action",
                            vlc_GetActionId( psz_arg ) );
        }
        else switch( psz_cmd[0] )
        {
        case 'f':
        case 'F':
        {
            bool fs;
            if( !strncasecmp( psz_arg, "on", 2 ) )
                var_SetBool( p_sys->p_playlist, "fullscreen", fs = true );
            else if( !strncasecmp( psz_arg, "off", 3 ) )
                var_SetBool( p_sys->p_playlist, "fullscreen", fs = false );
            else
                fs = var_ToggleBool( p_sys->p_playlist, "fullscreen" );
            if( p_sys->p_input != NULL )
            {
                vout_thread_t *p_vout = input_GetVout( p_sys->p_input );
                if( p_vout )
                {
                    var_SetBool( p_vout, "fullscreen", fs );
                    vlc_object_release( p_vout );
                }
            }
            break;
        }
        case 's':
        case 'S':
            ;
            break;
        case '\0':
            /* Ignore empty lines */
            break;
        default:
            msg_rc(_("Unknown command `%s'. Type `help' for help."), psz_cmd);
            break;
        }
        /* Command processed */
        i_size = 0; p_buffer[0] = 0;
    }
    msg_rc( STATUS_CHANGE "( stop state: 0 )" );
    msg_rc( STATUS_CHANGE "( quit )" );
    vlc_restorecancel( canc );
    return NULL;
}
