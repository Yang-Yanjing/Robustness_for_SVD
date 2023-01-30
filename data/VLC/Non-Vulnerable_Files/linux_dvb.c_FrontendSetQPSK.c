}
static int FrontendSetQPSK( access_t *p_access )
{
    access_sys_t *p_sys = p_access->p_sys;
    struct dvb_frontend_parameters fep;
    int i_val;
    int i_frequency, i_lnb_slof = 0, i_lnb_lof1, i_lnb_lof2 = 0;
    /* Prepare the fep structure */
    i_frequency = var_GetInteger( p_access, "dvb-frequency" );
    i_val = var_GetInteger( p_access, "dvb-lnb-lof1" );
    if( i_val == 0 )
    {
        /* Automatic mode. */
        if ( i_frequency >= 950000 && i_frequency <= 2150000 )
        {
            msg_Dbg( p_access, "frequency %d is in IF-band", i_frequency );
            i_lnb_lof1 = 0;
        }
        else if ( i_frequency >= 2500000 && i_frequency <= 2700000 )
        {
            msg_Dbg( p_access, "frequency %d is in S-band", i_frequency );
            i_lnb_lof1 = 3650000;
        }
        else if ( i_frequency >= 3400000 && i_frequency <= 4200000 )
        {
            msg_Dbg( p_access, "frequency %d is in C-band (lower)",
                     i_frequency );
            i_lnb_lof1 = 5150000;
        }
        else if ( i_frequency >= 4500000 && i_frequency <= 4800000 )
        {
            msg_Dbg( p_access, "frequency %d is in C-band (higher)",
                     i_frequency );
            i_lnb_lof1 = 5950000;
        }
        else if ( i_frequency >= 10700000 && i_frequency <= 13250000 )
        {
            msg_Dbg( p_access, "frequency %d is in Ku-band",
                     i_frequency );
            i_lnb_lof1 = 9750000;
            i_lnb_lof2 = 10600000;
            i_lnb_slof = 11700000;
        }
        else
        {
            msg_Err( p_access, "frequency %d is out of any known band",
                     i_frequency );
            msg_Err( p_access, "specify dvb-lnb-lof1 manually for the local "
                     "oscillator frequency" );
            return VLC_EGENERIC;
        }
        var_SetInteger( p_access, "dvb-lnb-lof1", i_lnb_lof1 );
        var_SetInteger( p_access, "dvb-lnb-lof2", i_lnb_lof2 );
        var_SetInteger( p_access, "dvb-lnb-slof", i_lnb_slof );
    }
    else
    {
        i_lnb_lof1 = i_val;
        i_lnb_lof2 = var_GetInteger( p_access, "dvb-lnb-lof2" );
        i_lnb_slof = var_GetInteger( p_access, "dvb-lnb-slof" );
    }
    if( i_lnb_slof && i_frequency >= i_lnb_slof )
    {
        i_frequency -= i_lnb_lof2;
    }
    else
    {
        i_frequency -= i_lnb_lof1;
    }
    fep.frequency = i_frequency >= 0 ? i_frequency : -i_frequency;
    fep.inversion = DecodeInversion( p_access );
    fep.u.qpsk.symbol_rate = var_GetInteger( p_access, "dvb-srate" );
    fep.u.qpsk.fec_inner = FEC_NONE;
    if( DoDiseqc( p_access ) < 0 )
    {
        return VLC_EGENERIC;
    }
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
