} vlc_ipc_data_t;
void system_Configure( libvlc_int_t *p_this, int i_argc, const char *const ppsz_argv[] )
{
#if !VLC_WINSTORE_APP
    /* Raise default priority of the current process */
#ifndef ABOVE_NORMAL_PRIORITY_CLASS
#   define ABOVE_NORMAL_PRIORITY_CLASS 0x00008000
#endif
    if( var_InheritBool( p_this, "high-priority" ) )
    {
        if( SetPriorityClass( GetCurrentProcess(), ABOVE_NORMAL_PRIORITY_CLASS )
             || SetPriorityClass( GetCurrentProcess(), HIGH_PRIORITY_CLASS ) )
        {
            msg_Dbg( p_this, "raised process priority" );
        }
        else
        {
            msg_Dbg( p_this, "could not raise process priority" );
        }
    }
    if( var_InheritBool( p_this, "one-instance" )
     || ( var_InheritBool( p_this, "one-instance-when-started-from-file" )
       && var_InheritBool( p_this, "started-from-file" ) ) )
    {
        HANDLE hmutex;
        msg_Info( p_this, "one instance mode ENABLED");
        /* Use a named mutex to check if another instance is already running */
        if( !( hmutex = CreateMutex( 0, TRUE, L"VLC ipc " TEXT(VERSION) ) ) )
        {
            /* Failed for some reason. Just ignore the option and go on as
             * normal. */
            msg_Err( p_this, "one instance mode DISABLED "
                     "(mutex couldn't be created)" );
            return;
        }
        if( GetLastError() != ERROR_ALREADY_EXISTS )
        {
            libvlc_InternalAddIntf( p_this, "win_msg,none" );
            /* Initialization done.
             * Release the mutex to unblock other instances */
            ReleaseMutex( hmutex );
        }
        else
        {
            /* Another instance is running */
            HWND ipcwindow;
            /* Wait until the 1st instance is initialized */
            WaitForSingleObject( hmutex, INFINITE );
            /* Locate the window created by the IPC helper thread of the
             * 1st instance */
            if( !( ipcwindow = FindWindow( 0, L"VLC ipc " TEXT(VERSION) ) ) )
            {
                msg_Err( p_this, "one instance mode DISABLED "
                         "(couldn't find 1st instance of program)" );
                ReleaseMutex( hmutex );
                return;
            }
            /* We assume that the remaining parameters are filenames
             * and their input options */
            if( i_argc > 0 )
            {
                COPYDATASTRUCT wm_data;
                int i_opt;
                vlc_ipc_data_t *p_data;
                size_t i_data = sizeof (*p_data);
                for( i_opt = 0; i_opt < i_argc; i_opt++ )
                {
                    i_data += sizeof (size_t);
                    i_data += strlen( ppsz_argv[ i_opt ] ) + 1;
                }
                p_data = malloc( i_data );
                p_data->argc = i_argc;
                p_data->enqueue = var_InheritBool( p_this, "playlist-enqueue" );
                i_data = 0;
                for( i_opt = 0; i_opt < i_argc; i_opt++ )
                {
                    size_t i_len = strlen( ppsz_argv[ i_opt ] ) + 1;
                    /* Windows will never switch to an architecture
                     * with stronger alignment requirements, right. */
                    *((size_t *)(p_data->data + i_data)) = i_len;
                    i_data += sizeof (size_t);
                    memcpy( &p_data->data[i_data], ppsz_argv[ i_opt ], i_len );
                    i_data += i_len;
                }
                i_data += sizeof (*p_data);
                /* Send our playlist items to the 1st instance */
                wm_data.dwData = 0;
                wm_data.cbData = i_data;
                wm_data.lpData = p_data;
                SendMessage( ipcwindow, WM_COPYDATA, 0, (LPARAM)&wm_data );
            }
            /* Initialization done.
             * Release the mutex to unblock other instances */
            ReleaseMutex( hmutex );
            /* Bye bye */
            system_End( );
            exit( 0 );
        }
    }
#endif
}
