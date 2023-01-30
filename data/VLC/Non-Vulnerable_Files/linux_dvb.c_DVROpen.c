 *****************************************************************************/
int DVROpen( access_t * p_access )
{
    access_sys_t *p_sys = p_access->p_sys;
    unsigned int i_adapter, i_device;
    char dvr[128];
    i_adapter = var_GetInteger( p_access, "dvb-adapter" );
    i_device = var_GetInteger( p_access, "dvb-device" );
    if( snprintf( dvr, sizeof(dvr), DVR, i_adapter, i_device )
            >= (int)sizeof(dvr) )
    {
        msg_Err( p_access, "snprintf() truncated string for DVR" );
        dvr[sizeof(dvr) - 1] = '\0';
    }
    msg_Dbg( p_access, "Opening device %s", dvr );
    if( (p_sys->i_handle = vlc_open(dvr, O_RDONLY)) < 0 )
    {
        msg_Err( p_access, "DVROpen: opening device failed: %s",
                 vlc_strerror_c(errno) );
        return VLC_EGENERIC;
    }
    if( fcntl( p_sys->i_handle, F_SETFL, O_NONBLOCK ) == -1 )
    {
        msg_Warn( p_access, "DVROpen: couldn't set non-blocking mode: %s",
                  vlc_strerror_c(errno) );
    }
    return VLC_SUCCESS;
}
