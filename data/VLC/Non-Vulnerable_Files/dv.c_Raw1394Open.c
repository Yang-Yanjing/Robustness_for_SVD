}
static raw1394handle_t Raw1394Open( access_t *p_access, int port )
{
    int n_ports;
    struct raw1394_portinfo pinf[ 16 ];
    raw1394handle_t handle;
    /* get a raw1394 handle */
    handle = raw1394_new_handle();
    if( !handle )
    {
        msg_Err( p_access, "raw1394 - failed to get handle: %s",
                 vlc_strerror_c(errno) );
        return NULL;
    }
    if( ( n_ports = raw1394_get_port_info( handle, pinf, 16 ) ) < 0 )
    {
        msg_Err( p_access, "raw1394 - failed to get port info: %s",
                 vlc_strerror_c(errno) );
        raw1394_destroy_handle( handle );
        return NULL;
    }
    /* tell raw1394 which host adapter to use */
    if( raw1394_set_port( handle, port ) < 0 )
    {
        msg_Err( p_access, "raw1394 - failed to set set port: %s",
                 vlc_strerror_c(errno) );
        return NULL;
    }
    return handle;
}
