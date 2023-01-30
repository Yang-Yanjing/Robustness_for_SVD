 *****************************************************************************/
static int Open( vlc_object_t *p_this )
{
    access_t     *p_access = (access_t*)p_this;
    access_sys_t *p_sys;
    struct raw1394_portinfo port_inf[ 16 ];
    msg_Dbg( p_access, "opening device" );
    /* Set up p_access */
    access_InitFields( p_access );
    ACCESS_SET_CALLBACKS( NULL, Block, Control, NULL );
    p_access->p_sys = p_sys = malloc( sizeof( access_sys_t ) );
    if( !p_sys )
        return VLC_EGENERIC;
    p_sys->i_cards = 0;
    p_sys->i_node = 0;
    p_sys->i_port = 0;
    p_sys->i_guid = 0;
    p_sys->i_channel = 63;
    p_sys->p_raw1394 = NULL;
    p_sys->p_avc1394 = NULL;
    p_sys->p_frame = NULL;
    p_sys->p_ev = NULL;
    vlc_mutex_init( &p_sys->lock );
    p_sys->i_node = DiscoverAVC( p_access, &p_sys->i_port, p_sys->i_guid );
    if( p_sys->i_node < 0 )
    {
        msg_Err( p_access, "failed to open a Firewire (IEEE1394) connection" );
        Close( p_this );
        return VLC_EGENERIC;
    }
    p_sys->p_avc1394 = AVCOpen( p_access, p_sys->i_port );
    if( !p_sys->p_avc1394 )
    {
        msg_Err( p_access, "no Digital Video Control device found" );
        Close( p_this );
        return VLC_EGENERIC;
    }
    p_sys->p_raw1394 = raw1394_new_handle();
    if( !p_sys->p_raw1394 )
    {
        msg_Err( p_access, "no Digital Video device found" );
        Close( p_this );
        return VLC_EGENERIC;
    }
    p_sys->i_cards = raw1394_get_port_info( p_sys->p_raw1394, port_inf, 16 );
    if( p_sys->i_cards < 0 )
    {
        msg_Err( p_access, "failed to get port info" );
        Close( p_this );
        return VLC_EGENERIC;
    }
    if( raw1394_set_port( p_sys->p_raw1394, p_sys->i_port ) < 0 )
    {
        msg_Err( p_access, "failed to set port info" );
        Close( p_this );
        return VLC_EGENERIC;
    }
    if ( raw1394_iso_recv_init( p_sys->p_raw1394, Raw1394Handler,
                ISOCHRONOUS_QUEUE_LENGTH, ISOCHRONOUS_MAX_PACKET_SIZE,
                p_sys->i_channel, RAW1394_DMA_PACKET_PER_BUFFER, -1 ) < 0 )
    {
        msg_Err( p_access, "failed to init isochronous recv" );
        Close( p_this );
        return VLC_EGENERIC;
    }
    raw1394_set_userdata( p_sys->p_raw1394, p_access );
    raw1394_iso_recv_start( p_sys->p_raw1394, -1, -1, 0 );
    p_sys->raw1394_poll.fd = raw1394_get_fd( p_sys->p_raw1394 );
    p_sys->raw1394_poll.events = POLLIN | POLLPRI;
    /* Now create our event thread catcher */
    p_sys->p_ev = calloc( 1, sizeof( *p_sys->p_ev ) );
    if( !p_sys->p_ev )
    {
        msg_Err( p_access, "failed to create event thread struct" );
        Close( p_this );
        return VLC_ENOMEM;
    }
    p_sys->p_ev->p_frame = NULL;
    p_sys->p_ev->pp_last = &p_sys->p_ev->p_frame;
    p_sys->p_ev->p_access = p_access;
    vlc_mutex_init( &p_sys->p_ev->lock );
    if( vlc_clone( &p_sys->p_ev->thread, Raw1394EventThread,
               p_sys->p_ev, VLC_THREAD_PRIORITY_OUTPUT ) )
    {
        msg_Err( p_access, "failed to clone event thread" );
        Close( p_this );
        return VLC_EGENERIC;
    }
    return VLC_SUCCESS;
}
