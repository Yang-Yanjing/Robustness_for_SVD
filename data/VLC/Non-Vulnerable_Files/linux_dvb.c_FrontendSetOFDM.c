}
static int FrontendSetOFDM( access_t * p_access )
{
    access_sys_t *p_sys = p_access->p_sys;
    struct dvb_frontend_parameters fep;
    /* Prepare the fep structure */
    fep.frequency = var_GetInteger( p_access, "dvb-frequency" );
    fep.inversion = DecodeInversion( p_access );
    fep.u.ofdm.bandwidth = DecodeBandwidth( p_access );
    fep.u.ofdm.code_rate_HP = FEC_NONE;
    fep.u.ofdm.code_rate_LP = FEC_NONE;
    fep.u.ofdm.constellation = QAM_AUTO;
    fep.u.ofdm.transmission_mode = DecodeTransmission( p_access );
    fep.u.ofdm.guard_interval = GUARD_INTERVAL_AUTO;
    fep.u.ofdm.hierarchy_information = DecodeHierarchy( p_access );
    /* Empty the event queue */
    for( ; ; )
    {
        struct dvb_frontend_event event;
        if ( ioctl( p_sys->i_frontend_handle, FE_GET_EVENT, &event ) < 0
              && errno == EWOULDBLOCK )
            break;
    }
    /* Now send it all to the frontend device */
    if( ioctl( p_sys->i_frontend_handle, FE_SET_FRONTEND, &fep ) < 0 )
    {
        msg_Err( p_access, "frontend error: %s", vlc_strerror_c(errno) );
        return VLC_EGENERIC;
    }
    return VLC_SUCCESS;
}
