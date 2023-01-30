 *****************************************************************************/
static void DateTimeHandle( cam_t *p_cam, int i_session_id,
                            uint8_t *p_apdu, int i_size )
{
    date_time_t *p_date =
        (date_time_t *)p_cam->p_sessions[i_session_id - 1].p_sys;
    int i_tag = APDUGetTag( p_apdu, i_size );
    switch ( i_tag )
    {
    case AOT_DATE_TIME_ENQ:
    {
        int l;
        const uint8_t *d = APDUGetLength( p_apdu, &l );
        if ( l > 0 )
        {
            p_date->i_interval = *d;
            msg_Dbg( p_cam->obj, "DateTimeHandle : interval set to %d",
                     p_date->i_interval );
        }
        else
            p_date->i_interval = 0;
        DateTimeSend( p_cam, i_session_id );
        break;
    }
    default:
        msg_Err( p_cam->obj, "unexpected tag in DateTimeHandle (0x%x)",
                 i_tag );
    }
}
