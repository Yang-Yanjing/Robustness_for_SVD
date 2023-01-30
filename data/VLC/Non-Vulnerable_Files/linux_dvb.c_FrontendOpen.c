 *****************************************************************************/
int FrontendOpen( access_t *p_access )
{
    access_sys_t *p_sys = p_access->p_sys;
    frontend_t * p_frontend;
    unsigned int i_adapter, i_device;
    bool b_probe;
    char frontend[128];
    i_adapter = var_GetInteger( p_access, "dvb-adapter" );
    i_device = var_GetInteger( p_access, "dvb-device" );
    b_probe = var_GetBool( p_access, "dvb-probe" );
    if( snprintf( frontend, sizeof(frontend), FRONTEND, i_adapter, i_device ) >= (int)sizeof(frontend) )
    {
        msg_Err( p_access, "snprintf() truncated string for FRONTEND" );
        frontend[sizeof(frontend) - 1] = '\0';
    }
    p_sys->p_frontend = p_frontend = malloc( sizeof(frontend_t) );
    if( !p_frontend )
        return VLC_ENOMEM;
    msg_Dbg( p_access, "Opening device %s", frontend );
    if( (p_sys->i_frontend_handle = vlc_open(frontend, O_RDWR | O_NONBLOCK)) < 0 )
    {
        msg_Err( p_access, "FrontEndOpen: opening device failed: %s",
                 vlc_strerror_c(errno) );
        free( p_frontend );
        return VLC_EGENERIC;
    }
    if( b_probe )
    {
        const char * psz_expected = NULL;
        const char * psz_real;
        if( FrontendInfo( p_access ) < 0 )
        {
            close( p_sys->i_frontend_handle );
            free( p_frontend );
            return VLC_EGENERIC;
        }
        switch( p_frontend->info.type )
        {
        case FE_OFDM:
            psz_real = "DVB-T";
            break;
        case FE_QAM:
            psz_real = "DVB-C";
            break;
        case FE_QPSK:
            psz_real = "DVB-S";
            break;
        case FE_ATSC:
            psz_real = "ATSC";
            break;
        default:
            psz_real = "unknown";
        }
        /* Sanity checks */
        if( (!strncmp( p_access->psz_access, "qpsk", 4 ) ||
             !strncmp( p_access->psz_access, "dvb-s", 5 ) ||
             !strncmp( p_access->psz_access, "satellite", 9 ) ) &&
             (p_frontend->info.type != FE_QPSK) )
        {
            psz_expected = "DVB-S";
        }
        if( (!strncmp( p_access->psz_access, "cable", 5 ) ||
             !strncmp( p_access->psz_access, "dvb-c", 5 ) ) &&
             (p_frontend->info.type != FE_QAM) )
        {
            psz_expected = "DVB-C";
        }
        if( (!strncmp( p_access->psz_access, "terrestrial", 11 ) ||
             !strncmp( p_access->psz_access, "dvb-t", 5 ) ) &&
             (p_frontend->info.type != FE_OFDM) )
        {
            psz_expected = "DVB-T";
        }
        if( (!strncmp( p_access->psz_access, "usdigital", 9 ) ||
             !strncmp( p_access->psz_access, "atsc", 4 ) ) &&
             (p_frontend->info.type != FE_ATSC) )
        {
            psz_expected = "ATSC";
        }
        if( psz_expected != NULL )
        {
            msg_Err( p_access, "requested type %s not supported by %s tuner",
                     psz_expected, psz_real );
            close( p_sys->i_frontend_handle );
            free( p_frontend );
            return VLC_EGENERIC;
        }
    }
    else /* no frontend probing is done so use default border values. */
    {
        msg_Dbg( p_access, "using default values for frontend info" );
        msg_Dbg( p_access, "method of access is %s", p_access->psz_access );
        p_frontend->info.type = FE_QPSK;
        if( !strncmp( p_access->psz_access, "qpsk", 4 ) ||
            !strncmp( p_access->psz_access, "dvb-s", 5 ) )
            p_frontend->info.type = FE_QPSK;
        else if( !strncmp( p_access->psz_access, "cable", 5 ) ||
                 !strncmp( p_access->psz_access, "dvb-c", 5 ) )
            p_frontend->info.type = FE_QAM;
        else if( !strncmp( p_access->psz_access, "terrestrial", 11 ) ||
                 !strncmp( p_access->psz_access, "dvb-t", 5 ) )
            p_frontend->info.type = FE_OFDM;
        else if( !strncmp( p_access->psz_access, "usdigital", 9 ) ||
                 !strncmp( p_access->psz_access, "atsc", 4 ) )
            p_frontend->info.type = FE_ATSC;
    }
    return VLC_SUCCESS;
}
