}
int FrontendGetStatistic( access_t *p_access, frontend_statistic_t *p_stat )
{
    access_sys_t *p_sys = p_access->p_sys;
    frontend_t * p_frontend = p_sys->p_frontend;
    if( (p_frontend->i_last_status & FE_HAS_LOCK) == 0 )
        return VLC_EGENERIC;
    memset( p_stat, 0, sizeof(*p_stat) );
    if( ioctl( p_sys->i_frontend_handle, FE_READ_BER, &p_stat->i_ber ) < 0 )
        p_stat->i_ber = -1;
    if( ioctl( p_sys->i_frontend_handle, FE_READ_SIGNAL_STRENGTH, &p_stat->i_signal_strenth ) < 0 )
        p_stat->i_signal_strenth = -1;
    if( ioctl( p_sys->i_frontend_handle, FE_READ_SNR, &p_stat->i_snr ) < 0 )
        p_stat->i_snr = -1;
    return VLC_SUCCESS;
}
