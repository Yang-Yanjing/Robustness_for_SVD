}
static bool open_vnc_connection ( filter_t *p_filter )
{
    filter_sys_t *p_sys = p_filter->p_sys;
    msg_Dbg( p_filter, "Open socket to vnc server on %s:%u.",
              p_sys->psz_host, p_sys->i_port );
    p_sys->i_socket = net_ConnectTCP( p_filter, p_sys->psz_host, p_sys->i_port );
    if( p_sys->i_socket < 0 )
    {
        msg_Err( p_filter, "Could not open socket" );
        return false;
    }
    msg_Dbg( p_filter, "socket is open." );
    return true;
}
