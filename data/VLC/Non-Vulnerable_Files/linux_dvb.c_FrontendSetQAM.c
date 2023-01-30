 *****************************************************************************/
static int FrontendSetQAM( access_t *p_access )
{
    access_sys_t *p_sys = p_access->p_sys;
    frontend_t *p_frontend = p_sys->p_frontend;
    struct dvb_frontend_parameters fep;
    unsigned int i_val;
    /* Prepare the fep structure */
    fep.frequency = var_GetInteger( p_access, "dvb-frequency" );
    fep.inversion = DecodeInversion( p_access );
    /* Default symbol-rate is for dvb-s, and doesn't fit
     * for dvb-c, so if it's over the limit of frontend, default to
     * somewhat common value
     */
    i_val = var_GetInteger( p_access, "dvb-srate" );
    if( i_val < p_frontend->info.symbol_rate_max &&
        i_val > p_frontend->info.symbol_rate_min )
        fep.u.qam.symbol_rate = i_val;
    fep.u.qam.fec_inner = FEC_NONE;
    fep.u.qam.modulation = QAM_AUTO;
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
