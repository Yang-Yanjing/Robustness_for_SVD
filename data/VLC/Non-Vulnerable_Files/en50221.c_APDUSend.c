 *****************************************************************************/
static int APDUSend( cam_t * p_cam, int i_session_id, int i_tag,
                     uint8_t *p_data, int i_size )
{
    uint8_t *p_apdu = xmalloc( i_size + 12 );
    uint8_t *p = p_apdu;
    ca_msg_t ca_msg;
    int i_ret;
    *p++ = (i_tag >> 16);
    *p++ = (i_tag >> 8) & 0xff;
    *p++ = i_tag & 0xff;
    p = SetLength( p, i_size );
    if ( i_size )
        memcpy( p, p_data, i_size );
    if ( p_cam->i_ca_type == CA_CI_LINK )
    {
        i_ret = SPDUSend( p_cam, i_session_id, p_apdu, i_size + p - p_apdu );
    }
    else
    {
        if ( i_size + p - p_apdu > 256 )
        {
            msg_Err( p_cam->obj, "CAM: apdu overflow" );
            i_ret = VLC_EGENERIC;
        }
        else
        {
            ca_msg.length = i_size + p - p_apdu;
            if ( i_size == 0 ) ca_msg.length=3;
            memcpy( ca_msg.msg, p_apdu, i_size + p - p_apdu );
            i_ret = ioctl( p_cam->fd, CA_SEND_MSG, &ca_msg );
            if ( i_ret < 0 )
            {
                msg_Err( p_cam->obj, "Error sending to CAM: %s",
                         vlc_strerror_c(errno) );
                i_ret = VLC_EGENERIC;
            }
        }
    }
    free( p_apdu );
    return i_ret;
}
