 *****************************************************************************/
static void ApplicationInformationOpen( cam_t * p_cam, unsigned i_session_id )
{
    msg_Dbg( p_cam->obj, "opening ApplicationInformation session (%u)",
             i_session_id );
    p_cam->p_sessions[i_session_id - 1].pf_handle =
        ApplicationInformationHandle;
    APDUSend( p_cam, i_session_id, AOT_APPLICATION_INFO_ENQ, NULL, 0 );
}
