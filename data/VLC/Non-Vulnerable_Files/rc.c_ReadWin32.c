#ifdef _WIN32
static bool ReadWin32( intf_thread_t *p_intf, char *p_buffer, int *pi_size )
{
    INPUT_RECORD input_record;
    DWORD i_dw;
    /* On Win32, select() only works on socket descriptors */
    while( WaitForSingleObjectEx( p_intf->p_sys->hConsoleIn,
                                INTF_IDLE_SLEEP/1000, TRUE ) == WAIT_OBJECT_0 )
    {
        while( *pi_size < MAX_LINE_LENGTH &&
               ReadConsoleInput( p_intf->p_sys->hConsoleIn, &input_record,
                                 1, &i_dw ) )
        {
            if( input_record.EventType != KEY_EVENT ||
                !input_record.Event.KeyEvent.bKeyDown ||
                input_record.Event.KeyEvent.wVirtualKeyCode == VK_SHIFT ||
                input_record.Event.KeyEvent.wVirtualKeyCode == VK_CONTROL||
                input_record.Event.KeyEvent.wVirtualKeyCode == VK_MENU ||
                input_record.Event.KeyEvent.wVirtualKeyCode == VK_CAPITAL )
            {
                /* nothing interesting */
                continue;
            }
            p_buffer[ *pi_size ] = input_record.Event.KeyEvent.uChar.AsciiChar;
            /* Echo out the command */
            putc( p_buffer[ *pi_size ], stdout );
            /* Handle special keys */
            if( p_buffer[ *pi_size ] == '\r' || p_buffer[ *pi_size ] == '\n' )
            {
                putc( '\n', stdout );
                break;
            }
            switch( p_buffer[ *pi_size ] )
            {
            case '\b':
                if( *pi_size )
                {
                    *pi_size -= 2;
                    putc( ' ', stdout );
                    putc( '\b', stdout );
                }
                break;
            case '\r':
                (*pi_size) --;
                break;
            }
            (*pi_size)++;
        }
        if( *pi_size == MAX_LINE_LENGTH ||
            p_buffer[ *pi_size ] == '\r' || p_buffer[ *pi_size ] == '\n' )
        {
            p_buffer[ *pi_size ] = 0;
            return true;
        }
    }
    vlc_testcancel ();
    return false;
}
