 *****************************************************************************/
static int FrontendSetATSC( access_t *p_access )
{
    access_sys_t *p_sys = p_access->p_sys;
    struct dvb_frontend_parameters fep;
    /* Prepare the fep structure */
    fep.frequency = var_GetInteger( p_access, "dvb-frequency" );
    fep.u.vsb.modulation = VSB_8;
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
