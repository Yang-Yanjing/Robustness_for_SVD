}
static int InitCapture( demux_t *p_demux )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    const int i_page_size = getpagesize();
    unsigned int i_bufmemsize;
    int i_ret;
    char psz_dev[MAXLEN];
    /* 10-bit mode or nothing */
    if ( WriteULSysfs( SDI_MODE_FILE, p_sys->i_link, SDI_CTL_MODE_10BIT ) < 0 )
    {
        msg_Err( p_demux, "couldn't write file " SDI_MODE_FILE, p_sys->i_link );
        return VLC_EGENERIC;
    }
    if ( (i_ret = ReadULSysfs( SDI_BUFFERS_FILE, p_sys->i_link )) < 0 )
    {
        msg_Err( p_demux, "couldn't read file " SDI_BUFFERS_FILE,
                 p_sys->i_link );
        return VLC_EGENERIC;
    }
    p_sys->i_buffers = i_ret;
    p_sys->i_current_buffer = 0;
    if ( (i_ret = ReadULSysfs( SDI_BUFSIZE_FILE, p_sys->i_link )) < 0 )
    {
        msg_Err( p_demux, "couldn't read file " SDI_BUFSIZE_FILE,
                 p_sys->i_link );
        return VLC_EGENERIC;
    }
    p_sys->i_buffer_size = i_ret;
    if ( p_sys->i_buffer_size % 20 )
    {
        msg_Err( p_demux, "buffer size must be a multiple of 20" );
        return VLC_EGENERIC;
    }
    snprintf( psz_dev, sizeof(psz_dev) - 1, SDI_DEVICE, p_sys->i_link );
    if ( (p_sys->i_fd = vlc_open( psz_dev, O_RDONLY ) ) < 0 )
    {
        msg_Err( p_demux, "couldn't open device %s", psz_dev );
        return VLC_EGENERIC;
    }
    i_bufmemsize = ((p_sys->i_buffer_size + i_page_size - 1) / i_page_size)
                     * i_page_size;
    p_sys->pp_buffers = malloc( p_sys->i_buffers * sizeof(uint8_t *) );
    if( !p_sys->pp_buffers )
        return VLC_ENOMEM;
    for ( unsigned int i = 0; i < p_sys->i_buffers; i++ )
    {
        if ( (p_sys->pp_buffers[i] = mmap( NULL, p_sys->i_buffer_size,
                                           PROT_READ, MAP_SHARED, p_sys->i_fd,
                                           i * i_bufmemsize )) == MAP_FAILED )
        {
            msg_Err( p_demux, "couldn't mmap(%d): %s", i,
                     vlc_strerror_c(errno) );
            free( p_sys->pp_buffers );
            return VLC_EGENERIC;
        }
    }
    return VLC_SUCCESS;
}
