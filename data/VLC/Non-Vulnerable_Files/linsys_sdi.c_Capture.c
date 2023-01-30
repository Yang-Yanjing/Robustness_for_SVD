}
static int Capture( demux_t *p_demux )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    struct pollfd pfd;
    pfd.fd = p_sys->i_fd;
    pfd.events = POLLIN | POLLPRI;
    if ( poll( &pfd, 1, READ_TIMEOUT ) < 0 )
    {
        msg_Warn( p_demux, "couldn't poll(): %s", vlc_strerror_c(errno) );
        return VLC_EGENERIC;
    }
    if ( pfd.revents & POLLPRI )
    {
        unsigned int i_val;
        if ( ioctl( p_sys->i_fd, SDI_IOC_RXGETEVENTS, &i_val ) < 0 )
            msg_Warn( p_demux, "couldn't SDI_IOC_RXGETEVENTS: %s",
                      vlc_strerror_c(errno) );
        else
        {
            if ( i_val & SDI_EVENT_RX_BUFFER )
                msg_Warn( p_demux, "driver receive buffer queue overrun" );
            if ( i_val & SDI_EVENT_RX_FIFO )
                msg_Warn( p_demux, "onboard receive FIFO overrun");
            if ( i_val & SDI_EVENT_RX_CARRIER )
                msg_Warn( p_demux, "carrier status change");
        }
        p_sys->i_next_date += CLOCK_GAP;
    }
    if ( pfd.revents & POLLIN )
    {
        int i_ret;
        if ( ioctl( p_sys->i_fd, SDI_IOC_DQBUF, p_sys->i_current_buffer ) < 0 )
        {
            msg_Warn( p_demux, "couldn't SDI_IOC_DQBUF: %s",
                      vlc_strerror_c(errno) );
            return VLC_EGENERIC;
        }
        i_ret = HandleSDBuffer( p_demux,
                                p_sys->pp_buffers[p_sys->i_current_buffer],
                                p_sys->i_buffer_size );
        if ( ioctl( p_sys->i_fd, SDI_IOC_QBUF, p_sys->i_current_buffer ) < 0 )
        {
            msg_Warn( p_demux, "couldn't SDI_IOC_QBUF: %s",
                      vlc_strerror_c(errno) );
            return VLC_EGENERIC;
        }
        if ( i_ret == VLC_SUCCESS )
        {
            p_sys->i_current_buffer++;
            p_sys->i_current_buffer %= p_sys->i_buffers;
        }
        else
        {
            /* Reference codes do not start on a multiple of 5. This sometimes
             * happen. We really don't want to allow this. */
            msg_Warn( p_demux, "resetting board" );
            CloseCapture( p_demux );
            InitCapture( p_demux );
        }
    }
    return VLC_SUCCESS;
}
