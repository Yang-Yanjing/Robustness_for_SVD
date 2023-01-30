 *****************************************************************************/
static int TPDUSend( cam_t * p_cam, uint8_t i_slot, uint8_t i_tag,
                     const uint8_t *p_content, size_t i_length )
{
    uint8_t p_data[9], *p = p_data;
    *(p++) = i_slot;
    *(p++) = i_slot + 1; /* TCID */
    *(p++) = i_tag;
    p = SetLength( p, i_length + 1 );
    *(p++) = i_slot + 1;
    Dump( true, p_data, p - p_data );
    const struct iovec iov[2] = {
        { p_data, p - p_data },
        { (void *)p_content, i_length },
    };
    if ( writev( p_cam->fd, iov, 2 ) <= 0 )
    {
        msg_Err( p_cam->obj, "cannot write to CAM device: %s",
                 vlc_strerror_c(errno) );
        return VLC_EGENERIC;
    }
    return VLC_SUCCESS;
}
