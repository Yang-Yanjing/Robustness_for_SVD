}
static int InitCapture( demux_t *p_demux )
{
    demux_sys_t *p_sys = p_demux->p_sys;
#ifdef HAVE_MMAP_SDIVIDEO
    const int i_page_size = getpagesize();
    unsigned int i_bufmemsize;
#endif
    char psz_vdev[MAXLEN];
    snprintf( psz_vdev, sizeof(psz_vdev), SDIVIDEO_DEVICE, p_sys->i_link );
    psz_vdev[sizeof(psz_vdev) - 1] = '\0';
    if ( (p_sys->i_vfd = vlc_open( psz_vdev, O_RDONLY ) ) < 0 )
    {
        msg_Err( p_demux, "couldn't open device %s", psz_vdev );
        return VLC_EGENERIC;
    }
    /* Wait for standard to settle down */
    while ( vlc_object_alive(p_demux) )
    {
        struct pollfd pfd[1];
        pfd[0].fd = p_sys->i_vfd;
        pfd[0].events = POLLIN | POLLPRI;
        if ( poll( pfd, 1, READ_TIMEOUT ) < 0 )
        {
            msg_Warn( p_demux, "couldn't poll(): %s", vlc_strerror_c(errno) );
            close( p_sys->i_vfd );
            return VLC_EGENERIC;
        }
        if ( pfd[0].revents & POLLPRI )
        {
            unsigned int i_val;
            if ( ioctl( p_sys->i_vfd, SDIVIDEO_IOC_RXGETEVENTS, &i_val ) < 0 )
                msg_Warn( p_demux, "couldn't SDIVIDEO_IOC_RXGETEVENTS: %s",
                          vlc_strerror_c(errno) );
            else
            {
                if ( i_val & SDIVIDEO_EVENT_RX_BUFFER )
                    msg_Warn( p_demux, "driver receive buffer queue overrun" );
                if ( i_val & SDIVIDEO_EVENT_RX_FIFO )
                    msg_Warn( p_demux, "onboard receive FIFO overrun");
                if ( i_val & SDIVIDEO_EVENT_RX_CARRIER )
                    msg_Warn( p_demux, "carrier status change");
                if ( i_val & SDIVIDEO_EVENT_RX_DATA )
                    msg_Warn( p_demux, "data status change");
                if ( i_val & SDIVIDEO_EVENT_RX_STD )
                {
                    msg_Warn( p_demux, "standard status change");
                    break;
                }
            }
        }
    }
    if ( !vlc_object_alive(p_demux) )
    {
        close( p_sys->i_vfd );
        return VLC_EGENERIC;
    }
    if ( ioctl( p_sys->i_vfd, SDIVIDEO_IOC_RXGETVIDSTATUS, &p_sys->i_standard )
          < 0 )
    {
        msg_Warn( p_demux, "couldn't SDIVIDEO_IOC_RXGETVIDSTATUS: %s",
                  vlc_strerror_c(errno) );
        close( p_sys->i_vfd );
        return VLC_EGENERIC;
    }
    close( p_sys->i_vfd );
    if ( InitVideo( p_demux ) != VLC_SUCCESS )
        return VLC_EGENERIC;
    p_sys->i_vbuffer_size = p_sys->i_height * p_sys->i_width * 2;
    /* First open the audio for synchronization reasons */
    if ( p_sys->i_max_channel != -1 )
    {
        unsigned int i_rate;
        char psz_adev[MAXLEN];
        snprintf( psz_adev, sizeof(psz_adev), SDIAUDIO_DEVICE, p_sys->i_link );
        psz_adev[sizeof(psz_adev) - 1] = '\0';
        if ( (p_sys->i_afd = vlc_open( psz_adev, O_RDONLY ) ) < 0 )
        {
            msg_Err( p_demux, "couldn't open device %s", psz_adev );
            return VLC_EGENERIC;
        }
        if ( ioctl( p_sys->i_afd, SDIAUDIO_IOC_RXGETAUDRATE, &i_rate ) < 0 )
        {
            msg_Warn( p_demux, "couldn't SDIAUDIO_IOC_RXGETAUDRATE: %s",
                      vlc_strerror_c(errno) );
            return VLC_EGENERIC;
        }
        switch ( i_rate )
        {
        case SDIAUDIO_CTL_ASYNC_48_KHZ:
        case SDIAUDIO_CTL_SYNC_48_KHZ:
            p_sys->i_sample_rate = 48000;
            break;
        case SDIAUDIO_CTL_ASYNC_44_1_KHZ:
        case SDIAUDIO_CTL_SYNC_44_1_KHZ:
            p_sys->i_sample_rate = 44100;
            break;
        case SDIAUDIO_CTL_ASYNC_32_KHZ:
        case SDIAUDIO_CTL_SYNC_32_KHZ:
            p_sys->i_sample_rate = 32000;
            break;
        case SDIAUDIO_CTL_ASYNC_96_KHZ:
        case SDIAUDIO_CTL_SYNC_96_KHZ:
            p_sys->i_sample_rate = 96000;
            break;
        case SDIAUDIO_CTL_ASYNC_FREE_RUNNING:
        case SDIAUDIO_CTL_SYNC_FREE_RUNNING:
        default:
            msg_Err( p_demux, "unknown sample rate %u", i_rate );
            return VLC_EGENERIC;
        }
        close( p_sys->i_afd );
        if ( InitAudio( p_demux ) != VLC_SUCCESS )
            return VLC_EGENERIC;
        p_sys->i_abuffer_size = p_sys->i_ablock_size
                                 * (1 + p_sys->i_max_channel);
        /* Use 16-bit audio */
        if ( WriteULSysfs( SDIAUDIO_SAMPLESIZE_FILE, p_sys->i_link,
                           SDIAUDIO_CTL_AUDSAMP_SZ_16 ) < 0 )
        {
            msg_Err( p_demux, "couldn't write file " SDIAUDIO_SAMPLESIZE_FILE,
                     p_sys->i_link );
            return VLC_EGENERIC;
        }
        if ( WriteULSysfs( SDIAUDIO_CHANNELS_FILE, p_sys->i_link,
                           (p_sys->i_max_channel + 1) * 2 ) < 0 )
        {
            msg_Err( p_demux, "couldn't write file " SDIAUDIO_CHANNELS_FILE,
                     p_sys->i_link );
            return VLC_EGENERIC;
        }
#ifdef HAVE_MMAP_SDIAUDIO
        if ( (p_sys->i_abuffers = ReadULSysfs( SDIAUDIO_BUFFERS_FILE,
                                               p_sys->i_link )) < 0 )
        {
            msg_Err( p_demux, "couldn't read file " SDIAUDIO_BUFFERS_FILE,
                     p_sys->i_link );
            return VLC_EGENERIC;
        }
        p_sys->i_current_abuffer = 0;
#endif
        if ( WriteULSysfs( SDIAUDIO_BUFSIZE_FILE, p_sys->i_link,
                           p_sys->i_abuffer_size ) < 0 )
        {
            msg_Err( p_demux, "couldn't write file " SDIAUDIO_BUFSIZE_FILE,
                     p_sys->i_link );
            return VLC_EGENERIC;
        }
        if ( (p_sys->i_afd = open( psz_adev, O_RDONLY ) ) < 0 )
        {
            msg_Err( p_demux, "couldn't open device %s", psz_adev );
            return VLC_EGENERIC;
        }
#ifdef HAVE_MMAP_SDIAUDIO
        i_bufmemsize = ((p_sys->i_abuffer_size + i_page_size - 1) / i_page_size)
                         * i_page_size;
        p_sys->pp_abuffers = malloc( p_sys->i_abuffers * sizeof(uint8_t *) );
        if( unlikely( !p_sys->pp_abuffers ) )
            return VLC_ENOMEM;
        for ( unsigned int i = 0; i < p_sys->i_abuffers; i++ )
        {
            if ( (p_sys->pp_abuffers[i] = mmap( NULL, p_sys->i_abuffer_size,
                                                PROT_READ, MAP_SHARED, p_sys->i_afd,
                                                i * i_bufmemsize )) == MAP_FAILED )
            {
                msg_Err( p_demux, "couldn't mmap(%d): %s", i,
                         vlc_strerror_c(errno) );
                return VLC_EGENERIC;
            }
        }
#endif
    }
    /* Use 8-bit video */
    if ( WriteULSysfs( SDIVIDEO_MODE_FILE, p_sys->i_link,
                       SDIVIDEO_CTL_MODE_UYVY ) < 0 )
    {
        msg_Err( p_demux, "couldn't write file " SDIVIDEO_MODE_FILE,
                 p_sys->i_link );
        return VLC_EGENERIC;
    }
    if ( WriteULSysfs( SDIVIDEO_BUFFERS_FILE, p_sys->i_link,
                       NB_VBUFFERS ) < 0 )
    {
        msg_Err( p_demux, "couldn't write file " SDIVIDEO_BUFFERS_FILE,
                 p_sys->i_link );
        return VLC_EGENERIC;
    }
#ifdef HAVE_MMAP_SDIVIDEO
    p_sys->i_vbuffers = NB_VBUFFERS;
#endif
    if ( WriteULSysfs( SDIVIDEO_BUFSIZE_FILE, p_sys->i_link,
                       p_sys->i_vbuffer_size ) < 0 )
    {
        msg_Err( p_demux, "couldn't write file " SDIVIDEO_BUFSIZE_FILE,
                 p_sys->i_link );
        return VLC_EGENERIC;
    }
    if ( (p_sys->i_vfd = open( psz_vdev, O_RDONLY ) ) < 0 )
    {
        msg_Err( p_demux, "couldn't open device %s", psz_vdev );
        return VLC_EGENERIC;
    }
#ifdef HAVE_MMAP_SDIVIDEO
    p_sys->i_current_vbuffer = 0;
    i_bufmemsize = ((p_sys->i_vbuffer_size + i_page_size - 1) / i_page_size)
                     * i_page_size;
    p_sys->pp_vbuffers = malloc( p_sys->i_vbuffers * sizeof(uint8_t *) );
    if( unlikely( !p_sys->pp_vbuffers ) )
        return VLC_ENOMEM;
    for ( unsigned int i = 0; i < p_sys->i_vbuffers; i++ )
    {
        if ( (p_sys->pp_vbuffers[i] = mmap( NULL, p_sys->i_vbuffer_size,
                                            PROT_READ, MAP_SHARED, p_sys->i_vfd,
                                            i * i_bufmemsize )) == MAP_FAILED )
        {
            msg_Err( p_demux, "couldn't mmap(%d): %s", i,
                     vlc_strerror_c(errno) );
            return VLC_EGENERIC;
        }
    }
#endif
    return VLC_SUCCESS;
}
