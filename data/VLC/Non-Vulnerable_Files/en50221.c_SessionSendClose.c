 *****************************************************************************/
static void SessionSendClose( cam_t *p_cam, int i_session_id )
{
    uint8_t p_response[16];
    uint8_t i_tag;
    uint8_t i_slot = p_cam->p_sessions[i_session_id - 1].i_slot;
    p_response[0] = ST_CLOSE_SESSION_REQUEST;
    p_response[1] = 0x2;
    SetWBE( &p_response[2], i_session_id );
    if ( TPDUSend( p_cam, i_slot, T_DATA_LAST, p_response, 4 ) !=
            VLC_SUCCESS )
    {
        msg_Err( p_cam->obj,
                 "SessionSendClose: couldn't send TPDU on slot %d", i_slot );
        return;
    }
    if ( TPDURecv( p_cam, i_slot, &i_tag, NULL, NULL ) != VLC_SUCCESS )
    {
        msg_Err( p_cam->obj,
                 "SessionSendClose: couldn't recv TPDU on slot %d", i_slot );
        return;
    }
}
