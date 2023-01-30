 *****************************************************************************/
static void SessionClose( cam_t * p_cam, int i_session_id )
{
    uint8_t p_response[16];
    uint8_t i_tag;
    uint8_t i_slot = p_cam->p_sessions[i_session_id - 1].i_slot;
    if ( p_cam->p_sessions[i_session_id - 1].pf_close != NULL )
        p_cam->p_sessions[i_session_id - 1].pf_close( p_cam, i_session_id );
    p_cam->p_sessions[i_session_id - 1].i_resource_id = 0;
    p_response[0] = ST_CLOSE_SESSION_RESPONSE;
    p_response[1] = 0x3;
    p_response[2] = SS_OK;
    p_response[3] = i_session_id >> 8;
    p_response[4] = i_session_id & 0xff;
    if ( TPDUSend( p_cam, i_slot, T_DATA_LAST, p_response, 5 ) !=
            VLC_SUCCESS )
    {
        msg_Err( p_cam->obj,
                 "SessionClose: couldn't send TPDU on slot %d", i_slot );
        return;
    }
    if ( TPDURecv( p_cam, i_slot, &i_tag, NULL, NULL ) != VLC_SUCCESS )
    {
        msg_Err( p_cam->obj,
                 "SessionClose: couldn't recv TPDU on slot %d", i_slot );
        return;
    }
}
