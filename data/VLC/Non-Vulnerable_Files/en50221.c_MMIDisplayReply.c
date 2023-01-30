 *****************************************************************************/
static void MMIDisplayReply( cam_t *p_cam, int i_session_id )
{
    uint8_t p_response[2];
    p_response[0] = DRI_MMI_MODE_ACK;
    p_response[1] = MM_HIGH_LEVEL;
    APDUSend( p_cam, i_session_id, AOT_DISPLAY_REPLY, p_response, 2 );
    msg_Dbg( p_cam->obj, "sending DisplayReply on session (%d)", i_session_id );
}
