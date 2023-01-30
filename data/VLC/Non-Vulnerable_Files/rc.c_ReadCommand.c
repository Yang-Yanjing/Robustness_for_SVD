#endif
bool ReadCommand( intf_thread_t *p_intf, char *p_buffer, int *pi_size )
{
    int i_read = 0;
#ifdef _WIN32
    if( p_intf->p_sys->i_socket == -1 && !p_intf->p_sys->b_quiet )
        return ReadWin32( p_intf, p_buffer, pi_size );
    else if( p_intf->p_sys->i_socket == -1 )
    {
        msleep( INTF_IDLE_SLEEP );
        return false;
    }
#endif
    while( *pi_size < MAX_LINE_LENGTH &&
           (i_read = net_Read( p_intf, p_intf->p_sys->i_socket == -1 ?
                       0 /*STDIN_FILENO*/ : p_intf->p_sys->i_socket, NULL,
                  (uint8_t *)p_buffer + *pi_size, 1, false ) ) > 0 )
    {
        if( p_buffer[ *pi_size ] == '\r' || p_buffer[ *pi_size ] == '\n' )
            break;
        (*pi_size)++;
    }
    /* Connection closed */
    if( i_read <= 0 )
    {
        if( p_intf->p_sys->i_socket != -1 )
        {
            net_Close( p_intf->p_sys->i_socket );
            p_intf->p_sys->i_socket = -1;
        }
        else
        {
            /* Standard input closed: exit */
            vlc_value_t empty;
            Quit( VLC_OBJECT(p_intf), NULL, empty, empty, NULL );
        }
        p_buffer[ *pi_size ] = 0;
        return true;
    }
    if( *pi_size == MAX_LINE_LENGTH ||
        p_buffer[ *pi_size ] == '\r' || p_buffer[ *pi_size ] == '\n' )
    {
        p_buffer[ *pi_size ] = 0;
        return true;
    }
    return false;
}
