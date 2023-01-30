}
static int Capture( demux_t *p_demux )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    struct pollfd pfd[2];
    pfd[0].fd = p_sys->i_vfd;
    pfd[0].events = POLLIN | POLLPRI;
    if ( p_sys->i_max_channel != -1 )
    {
        pfd[1].fd = p_sys->i_afd;
        pfd[1].events = POLLIN | POLLPRI;
    }
    if ( poll( pfd, 1 + (p_sys->i_max_channel != -1), READ_TIMEOUT ) < 0 )
    {
        msg_Warn( p_demux, "couldn't poll(): %s", vlc_strerror_c(errno) );
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
                msg_Warn( p_demux, "standard status change");
        }
        p_sys->i_next_adate += CLOCK_GAP;
        p_sys->i_next_vdate += CLOCK_GAP;
    }
    if ( p_sys->i_max_channel != -1 && pfd[1].revents & POLLPRI )
    {
        unsigned int i_val;
        if ( ioctl( p_sys->i_afd, SDIAUDIO_IOC_RXGETEVENTS, &i_val ) < 0 )
            msg_Warn( p_demux, "couldn't SDIAUDIO_IOC_RXGETEVENTS: %s",
                      vlc_strerror_c(errno) );
        else
        {
            if ( i_val & SDIAUDIO_EVENT_RX_BUFFER )
                msg_Warn( p_demux, "driver receive buffer queue overrun" );
            if ( i_val & SDIAUDIO_EVENT_RX_FIFO )
                msg_Warn( p_demux, "onboard receive FIFO overrun");
            if ( i_val & SDIAUDIO_EVENT_RX_CARRIER )
                msg_Warn( p_demux, "carrier status change");
            if ( i_val & SDIAUDIO_EVENT_RX_DATA )
                msg_Warn( p_demux, "data status change");
        }
        p_sys->i_next_adate += CLOCK_GAP;
        p_sys->i_next_vdate += CLOCK_GAP;
    }
    if ( pfd[0].revents & POLLIN )
    {
#ifdef HAVE_MMAP_SDIVIDEO
        if ( ioctl( p_sys->i_vfd, SDIVIDEO_IOC_DQBUF, p_sys->i_current_vbuffer )
              < 0 )
        {
            msg_Warn( p_demux, "couldn't SDIVIDEO_IOC_DQBUF: %s",
                      vlc_strerror_c(errno) );
            return VLC_EGENERIC;
        }
        if( HandleVideo( p_demux, p_sys->pp_vbuffers[p_sys->i_current_vbuffer] ) != VLC_SUCCESS )
            return VLC_ENOMEM;
        if ( ioctl( p_sys->i_vfd, SDIVIDEO_IOC_QBUF, p_sys->i_current_vbuffer )
              < 0 )
        {
            msg_Warn( p_demux, "couldn't SDIVIDEO_IOC_QBUF: %s",
                      vlc_strerror_c(errno) );
            return VLC_EGENERIC;
        }
        p_sys->i_current_vbuffer++;
        p_sys->i_current_vbuffer %= p_sys->i_vbuffers;
#else
        uint8_t *p_buffer = malloc( p_sys->i_vbuffer_size );
        if( unlikely( !p_buffer ) )
            return VLC_ENOMEM;
        if ( read( p_sys->i_vfd, p_buffer, p_sys->i_vbuffer_size ) < 0 )
        {
            msg_Warn( p_demux, "couldn't read: %s", vlc_strerror_c(errno) );
            free( p_buffer );
            return VLC_EGENERIC;
        }
        if( HandleVideo( p_demux, p_buffer ) != VLC_SUCCESS )
        {
            free( p_buffer );
            return VLC_ENOMEM;
        }
        free( p_buffer );
#endif
    }
    if ( p_sys->i_max_channel != -1 && pfd[1].revents & POLLIN )
    {
#ifdef HAVE_MMAP_SDIAUDIO
        if ( ioctl( p_sys->i_afd, SDIAUDIO_IOC_DQBUF, p_sys->i_current_abuffer )
              < 0 )
        {
            msg_Warn( p_demux, "couldn't SDIAUDIO_IOC_DQBUF: %s",
                      vlc_strerror_c(errno) );
            return VLC_EGENERIC;
        }
        if( HandleAudio( p_demux, p_sys->pp_abuffers[p_sys->i_current_abuffer] ) != VLC_SUCCESS )
            return VLC_ENOMEM;
        if ( ioctl( p_sys->i_afd, SDIAUDIO_IOC_QBUF, p_sys->i_current_abuffer )
              < 0 )
        {
            msg_Warn( p_demux, "couldn't SDIAUDIO_IOC_QBUF: %s",
                      vlc_strerror_c(errno) );
            return VLC_EGENERIC;
        }
        p_sys->i_current_abuffer++;
        p_sys->i_current_abuffer %= p_sys->i_abuffers;
#else
        uint8_t *p_buffer = malloc( p_sys->i_abuffer_size );
        if( unlikely( !p_buffer ) )
            return VLC_ENOMEM;
        if ( read( p_sys->i_afd, p_buffer, p_sys->i_abuffer_size ) < 0 )
        {
            msg_Warn( p_demux, "couldn't read: %s", vlc_strerror_c(errno) );
            free( p_buffer );
            return VLC_EGENERIC;
        }
        if( HandleAudio( p_demux, p_buffer ) != VLC_SUCCESS )
        {
            free( p_buffer );
            return VLC_ENOMEM;
        }
        free( p_buffer );
#endif
    }
    return VLC_SUCCESS;
}
