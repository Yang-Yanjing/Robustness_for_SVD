 */
static int Raw1394GetNumPorts( access_t *p_access )
{
    int n_ports;
    struct raw1394_portinfo pinf[ 16 ];
    raw1394handle_t handle;
    /* get a raw1394 handle */
    if( !( handle = raw1394_new_handle() ) )
    {
        msg_Err( p_access, "raw1394 - failed to get handle: %s",
                 vlc_strerror_c(errno) );
        return VLC_EGENERIC;
    }
    if( ( n_ports = raw1394_get_port_info( handle, pinf, 16 ) ) < 0 )
    {
        msg_Err( p_access, "raw1394 - failed to get port info: %s",
                 vlc_strerror_c(errno) );
        raw1394_destroy_handle( handle );
        return VLC_EGENERIC;
    }
    raw1394_destroy_handle( handle );
    return n_ports;
}
