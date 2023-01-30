 *****************************************************************************/
static void MMISendClose( cam_t *p_cam, int i_session_id )
{
    int i_slot = p_cam->p_sessions[i_session_id - 1].i_slot;
    APDUSend( p_cam, i_session_id, AOT_CLOSE_MMI, NULL, 0 );
    p_cam->pb_slot_mmi_expected[i_slot] = true;
}
