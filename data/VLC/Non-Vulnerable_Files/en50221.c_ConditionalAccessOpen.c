 *****************************************************************************/
static void ConditionalAccessOpen( cam_t * p_cam, unsigned i_session_id )
{
    msg_Dbg( p_cam->obj, "opening ConditionalAccess session (%u)",
             i_session_id );
    p_cam->p_sessions[i_session_id - 1].pf_handle = ConditionalAccessHandle;
    p_cam->p_sessions[i_session_id - 1].pf_close = ConditionalAccessClose;
    p_cam->p_sessions[i_session_id - 1].p_sys = calloc( 1, sizeof(system_ids_t) );
    APDUSend( p_cam, i_session_id, AOT_CA_INFO_ENQ, NULL, 0 );
}
