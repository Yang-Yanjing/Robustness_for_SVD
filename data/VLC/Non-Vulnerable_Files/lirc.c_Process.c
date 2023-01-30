}
static void Process( intf_thread_t *p_intf )
{
    for( ;; )
    {
        char *code, *c;
        if( lirc_nextcode( &code ) )
            return;
        if( code == NULL )
            return;
        while( (lirc_code2char( p_intf->p_sys->config, code, &c ) == 0)
                && (c != NULL) )
        {
            if( !strncmp( "key-", c, 4 ) )
            {
                vlc_action_t i_key = vlc_GetActionId( c );
                if( i_key )
                    var_SetInteger( p_intf->p_libvlc, "key-action", i_key );
                else
                    msg_Err( p_intf, "Unknown hotkey '%s'", c );
            }
            else
            {
                msg_Err( p_intf, "this doesn't appear to be a valid keycombo "
                                 "lirc sent us. Please look at the "
                                 "doc/lirc/example.lirc file in VLC" );
                break;
            }
        }
        free( code );
    }
}
