 *****************************************************************************/
static void ResourceManagerOpen( cam_t * p_cam, unsigned i_session_id )
{
    msg_Dbg( p_cam->obj, "opening ResourceManager session (%u)",
             i_session_id );
    p_cam->p_sessions[i_session_id - 1].pf_handle = ResourceManagerHandle;
    APDUSend( p_cam, i_session_id, AOT_PROFILE_ENQ, NULL, 0 );
}
