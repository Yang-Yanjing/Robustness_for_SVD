 */
int sout_AnnounceUnRegister( vlc_object_t *obj,
                             session_descriptor_t *p_session )
{
    sap_handler_t *p_sap = libvlc_priv (obj->p_libvlc)->p_sap;
    msg_Dbg (obj, "removing SAP session");
    SAP_Del (p_sap, p_session);
    vlc_mutex_lock (&sap_mutex);
    vlc_object_release ((vlc_object_t *)p_sap);
    vlc_mutex_unlock (&sap_mutex);
    free (p_session->psz_sdp);
    free (p_session);
    return 0;
}
