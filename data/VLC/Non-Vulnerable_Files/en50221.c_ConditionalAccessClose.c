 *****************************************************************************/
static void ConditionalAccessClose( cam_t * p_cam, int i_session_id )
{
    msg_Dbg( p_cam->obj, "closing ConditionalAccess session (%d)",
             i_session_id );
    free( p_cam->p_sessions[i_session_id - 1].p_sys );
}
