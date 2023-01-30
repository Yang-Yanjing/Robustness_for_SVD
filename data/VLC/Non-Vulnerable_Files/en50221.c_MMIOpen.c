 *****************************************************************************/
static void MMIOpen( cam_t *p_cam, unsigned i_session_id )
{
    mmi_t *p_mmi;
    msg_Dbg( p_cam->obj, "opening MMI session (%u)", i_session_id );
    p_cam->p_sessions[i_session_id - 1].pf_handle = MMIHandle;
    p_cam->p_sessions[i_session_id - 1].pf_close = MMIClose;
    p_cam->p_sessions[i_session_id - 1].p_sys = xmalloc(sizeof(mmi_t));
    p_mmi = (mmi_t *)p_cam->p_sessions[i_session_id - 1].p_sys;
    p_mmi->i_object_type = EN50221_MMI_NONE;
}
