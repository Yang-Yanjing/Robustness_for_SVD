    defined( HAVE_SCSIREQ_IN_SYS_SCSIIO_H )
static int CdTextRead( vlc_object_t *p_object, const vcddev_t *p_vcddev,
                       uint8_t **pp_buffer, int *pi_buffer )
{
    VLC_UNUSED( p_object );
    VLC_UNUSED( p_vcddev );
    VLC_UNUSED( pp_buffer );
    VLC_UNUSED( pi_buffer );
    return -1;
}
#elif defined( _WIN32 )
static int CdTextRead( vlc_object_t *p_object, const vcddev_t *p_vcddev,
                       uint8_t **pp_buffer, int *pi_buffer )
{
    VLC_UNUSED( p_object );
    CDROM_READ_TOC_EX TOCEx;
    memset(&TOCEx, 0, sizeof(TOCEx));
    TOCEx.Format = CDROM_READ_TOC_EX_FORMAT_CDTEXT;
    const int i_header_size = __MAX( 4, MINIMUM_CDROM_READ_TOC_EX_SIZE );
    uint8_t header[i_header_size];
    DWORD i_read;
    if( !DeviceIoControl( p_vcddev->h_device_handle, IOCTL_CDROM_READ_TOC_EX,
                          &TOCEx, sizeof(TOCEx), header, i_header_size, &i_read, 0 ) )
        return -1;
    const int i_text = 2 + (header[0] << 8) + header[1];
    if( i_text <= 4 )
        return -1;
    /* Read complete CD-TEXT */
    uint8_t *p_text = calloc( 1, i_text );
    if( !p_text )
        return VLC_EGENERIC;
    if( !DeviceIoControl( p_vcddev->h_device_handle, IOCTL_CDROM_READ_TOC_EX,
                          &TOCEx, sizeof(TOCEx), p_text, i_text, &i_read, 0 ) )
    {
        free( p_text );
        return VLC_EGENERIC;
    }
    /* */
    *pp_buffer = p_text;
    *pi_buffer = i_text;
    return VLC_SUCCESS;
}
#else
static int CdTextRead( vlc_object_t *p_object, const vcddev_t *p_vcddev,
                       uint8_t **pp_buffer, int *pi_buffer )
{
    VLC_UNUSED( p_object );
    if( p_vcddev->i_device_handle == -1 )
        return -1;
    struct cdrom_generic_command gc;
    uint8_t header[4];
    /* Read CD-TEXT size */
    memset( header, 0, sizeof(header) );
    memset( &gc, 0, sizeof(gc) );
    gc.cmd[0] = 0x43;   /* Read TOC */
    gc.cmd[1] = 0x02;   /* MSF */
    gc.cmd[2] = 5;      /* CD-Text */
    gc.cmd[7] = ( sizeof(header) >> 8 ) & 0xff;
    gc.cmd[8] = ( sizeof(header) >> 0 ) & 0xff;
    gc.buflen = sizeof(header);
    gc.buffer = header;
    gc.data_direction = CGC_DATA_READ;
    gc.timeout = 1000;
    if( ioctl( p_vcddev->i_device_handle, CDROM_SEND_PACKET, &gc ) == -1 )
        return VLC_EGENERIC;
    /* If the size is less than 4 it is an error, if it 4 then
     * it means no text data */
    const int i_text = 2 + (header[0] << 8) + header[1];
    if( i_text <= 4 )
        return VLC_EGENERIC;
    /* Read complete CD-TEXT */
    uint8_t *p_text = calloc( 1, i_text );
    if( !p_text )
        return VLC_EGENERIC;
    memset( &gc, 0, sizeof(gc) );
    gc.cmd[0] = 0x43;   /* Read TOC */
    gc.cmd[1] = 0x02;   /* MSF */
    gc.cmd[2] = 5;      /* CD-Text */
    gc.cmd[7] = ( i_text >> 8 ) & 0xff;
    gc.cmd[8] = ( i_text >> 0 ) & 0xff;
    gc.buflen = i_text;
    gc.buffer = p_text;
    gc.data_direction = CGC_DATA_READ;
    gc.timeout = 1000;
    if( ioctl( p_vcddev->i_device_handle, CDROM_SEND_PACKET, &gc ) == -1 )
    {
        free( p_text );
        return VLC_EGENERIC;
    }
    /* */
    *pp_buffer = p_text;
    *pi_buffer = i_text;
    return VLC_SUCCESS;
}
