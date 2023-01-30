 */
static raw1394handle_t AVCOpen( access_t *p_access, int port )
{
    access_sys_t *p_sys = p_access->p_sys;
    struct raw1394_portinfo pinf[ 16 ];
    int numcards;
    p_sys->p_avc1394 = raw1394_new_handle();
    if( !p_sys->p_avc1394 )
        return NULL;
    numcards = raw1394_get_port_info( p_sys->p_avc1394, pinf, 16 );
    if( numcards < -1 )
        return NULL;
    if( raw1394_set_port( p_sys->p_avc1394, port ) < 0 )
        return NULL;
    raw1394_set_bus_reset_handler( p_sys->p_avc1394, AVCResetHandler );
    return  p_sys->p_avc1394;
}
