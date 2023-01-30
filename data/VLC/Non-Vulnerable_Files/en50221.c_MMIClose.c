 *****************************************************************************/
static void MMIClose( cam_t *p_cam, int i_session_id )
{
    int i_slot = p_cam->p_sessions[i_session_id - 1].i_slot;
    mmi_t *p_mmi = (mmi_t *)p_cam->p_sessions[i_session_id - 1].p_sys;
    MMIFree( p_mmi );
    free( p_cam->p_sessions[i_session_id - 1].p_sys );
    msg_Dbg( p_cam->obj, "closing MMI session (%d)", i_session_id );
    p_cam->pb_slot_mmi_expected[i_slot] = false;
    p_cam->pb_slot_mmi_undisplayed[i_slot] = true;
}
