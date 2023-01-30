 *****************************************************************************/
cam_t *en50221_Init( vlc_object_t *obj, int fd )
{
    ca_caps_t caps;
    memset( &caps, 0, sizeof( caps ));
    if( ioctl( fd, CA_GET_CAP, &caps ) < 0 )
    {
        msg_Err( obj, "CAMInit: ioctl() error getting CAM capabilities" );
        return NULL;
    }
    /* Output CA capabilities */
    msg_Dbg( obj, "CA interface with %d slot(s)", caps.slot_num );
    if( caps.slot_type & CA_CI )
        msg_Dbg( obj, " CI high level interface type" );
    if( caps.slot_type & CA_CI_LINK )
        msg_Dbg( obj, " CI link layer level interface type" );
    if( caps.slot_type & CA_CI_PHYS )
        msg_Dbg( obj, " CI physical layer level interface type (not supported) " );
    if( caps.slot_type & CA_DESCR )
        msg_Dbg( obj, " built-in descrambler detected" );
    if( caps.slot_type & CA_SC )
        msg_Dbg( obj, " simple smart card interface" );
    msg_Dbg( obj, "%d available descrambler(s) (keys)", caps.descr_num );
    if( caps.descr_type & CA_ECD )
        msg_Dbg( obj, " ECD scrambling system supported" );
    if( caps.descr_type & CA_NDS )
        msg_Dbg( obj, " NDS scrambling system supported" );
    if( caps.descr_type & CA_DSS )
        msg_Dbg( obj, " DSS scrambling system supported" );
    if( caps.slot_num == 0 )
    {
        msg_Err( obj, "CAM module without slots" );
        return NULL;
    }
    cam_t *p_cam = calloc( 1, sizeof( *p_cam ) );
    if( unlikely(p_cam == NULL) )
        goto error;
    p_cam->obj = obj;
    p_cam->fd = fd;
    if( caps.slot_type & CA_CI_LINK )
    {
        p_cam->i_ca_type = CA_CI_LINK;
        for ( unsigned i_slot = 0; i_slot < p_cam->i_nb_slots; i_slot++ )
        {
            if ( ioctl( p_cam->fd, CA_RESET, 1 << i_slot) != 0 )
            {
                msg_Err( p_cam->obj, "en50221_Init: couldn't reset slot %d",
                         i_slot );
            }
        }
        p_cam->i_timeout = CLOCK_FREQ / 10;
        /* Wait a bit otherwise it doesn't initialize properly... */
        msleep( CLOCK_FREQ / 10 );
        p_cam->i_next_event = 0;
    }
    else
    if( caps.slot_type & CA_CI )
    {
        p_cam->i_ca_type = CA_CI;
        struct ca_slot_info info;
        info.num = 0;
        /* We don't reset the CAM in that case because it's done by the
         * ASIC. */
        if ( ioctl( fd, CA_GET_SLOT_INFO, &info ) < 0 )
        {
            msg_Err( obj, "cannot get slot info: %s", vlc_strerror_c(errno) );
            goto error;
        }
        if( info.flags == 0 )
        {
            msg_Err( obj, "no CAM inserted" );
            goto error;
        }
        /* Allocate a dummy sessions */
        p_cam->p_sessions[ 0 ].i_resource_id = RI_CONDITIONAL_ACCESS_SUPPORT;
        /* Get application info to find out which cam we are using and make
           sure everything is ready to play */
        ca_msg_t ca_msg;
        ca_msg.length=3;
        ca_msg.msg[0] = ( AOT_APPLICATION_INFO & 0xFF0000 ) >> 16;
        ca_msg.msg[1] = ( AOT_APPLICATION_INFO & 0x00FF00 ) >> 8;
        ca_msg.msg[2] = ( AOT_APPLICATION_INFO & 0x0000FF ) >> 0;
        memset( &ca_msg.msg[3], 0, 253 );
        APDUSend( p_cam, 1, AOT_APPLICATION_INFO_ENQ, NULL, 0 );
        if ( ioctl( fd, CA_GET_MSG, &ca_msg ) < 0 )
        {
            msg_Err( obj, "en50221_Init: failed getting message" );
            goto error;
        }
#if HLCI_WAIT_CAM_READY
        while( ca_msg.msg[8] == 0xff && ca_msg.msg[9] == 0xff )
        {
            if( !vlc_object_alive (obj) )
                goto error;
            msleep(1);
            msg_Dbg( obj, "CAM: please wait" );
            APDUSend( p_cam, 1, AOT_APPLICATION_INFO_ENQ, NULL, 0 );
            ca_msg.length=3;
            ca_msg.msg[0] = ( AOT_APPLICATION_INFO & 0xFF0000 ) >> 16;
            ca_msg.msg[1] = ( AOT_APPLICATION_INFO & 0x00FF00 ) >> 8;
            ca_msg.msg[2] = ( AOT_APPLICATION_INFO & 0x0000FF ) >> 0;
            memset( &ca_msg.msg[3], 0, 253 );
            if ( ioctl( fd, CA_GET_MSG, &ca_msg ) < 0 )
            {
                msg_Err( obj, "en50221_Init: failed getting message" );
                goto error;
            }
            msg_Dbg( p_cam->obj, "en50221_Init: Got length: %d, tag: 0x%x", ca_msg.length, APDUGetTag( ca_msg.msg, ca_msg.length ) );
        }
#else
        if( ca_msg.msg[8] == 0xff && ca_msg.msg[9] == 0xff )
        {
            msg_Err( obj, "CAM returns garbage as application info!" );
            goto error;
        }
#endif
        msg_Dbg( obj, "found CAM %s using id 0x%x", &ca_msg.msg[12],
                 (ca_msg.msg[8]<<8)|ca_msg.msg[9] );
    }
    else
    {
        msg_Err( obj, "CAM interface incompatible" );
        goto error;
    }
    return p_cam;
error:
    free( p_cam );
    return NULL;
}
