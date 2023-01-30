 *****************************************************************************/
int FrontendSet( access_t *p_access )
{
    access_sys_t *p_sys = p_access->p_sys;
    switch( p_sys->p_frontend->info.type )
    {
    /* DVB-S */
    case FE_QPSK:
        if( FrontendSetQPSK( p_access ) )
        {
            msg_Err( p_access, "DVB-S tuning error" );
            return VLC_EGENERIC;
        }
        break;
    /* DVB-C */
    case FE_QAM:
        if( FrontendSetQAM( p_access ) )
        {
            msg_Err( p_access, "DVB-C tuning error" );
            return VLC_EGENERIC;
        }
        break;
    /* DVB-T */
    case FE_OFDM:
        if( FrontendSetOFDM( p_access ) )
        {
            msg_Err( p_access, "DVB-T tuning error" );
            return VLC_EGENERIC;
        }
        break;
    /* ATSC */
    case FE_ATSC:
        if( FrontendSetATSC( p_access ) )
        {
            msg_Err( p_access, "ATSC tuning error" );
            return VLC_EGENERIC;
        }
        break;
    default:
        msg_Err( p_access, "tuner type %s not supported",
                 p_sys->p_frontend->info.name );
        return VLC_EGENERIC;
    }
    p_sys->p_frontend->i_last_status = 0;
    p_sys->i_frontend_timeout = mdate() + FRONTEND_LOCK_TIMEOUT;
    return VLC_SUCCESS;
}
