 *****************************************************************************/
static void DateTimeOpen( cam_t * p_cam, unsigned i_session_id )
{
    msg_Dbg( p_cam->obj, "opening DateTime session (%u)", i_session_id );
    p_cam->p_sessions[i_session_id - 1].pf_handle = DateTimeHandle;
    p_cam->p_sessions[i_session_id - 1].pf_manage = DateTimeManage;
    p_cam->p_sessions[i_session_id - 1].pf_close = DateTimeClose;
    p_cam->p_sessions[i_session_id - 1].p_sys = calloc( 1, sizeof(date_time_t) );
    DateTimeSend( p_cam, i_session_id );
}
