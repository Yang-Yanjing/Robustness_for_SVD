 */
static int st_ServerSessionOpen (vlc_tls_creds_t *crd, vlc_tls_t *session,
                                 int fd, const char *hostname) {
    VLC_UNUSED(hostname);
    msg_Dbg(session, "open TLS server session");
    int ret = st_SessionOpenCommon(crd, session, fd, true);
    if (ret != noErr) {
        goto error;
    }
    vlc_tls_sys_t *sys = session->sys;
    sys->b_server_mode = true;
    ret = SSLSetCertificate(sys->p_context, crd->sys->server_cert_chain);
    if (ret != noErr) {
        msg_Err(session, "cannot set server certificate");
        goto error;
    }
    return VLC_SUCCESS;
error:
    st_SessionClose(crd, session);
    return VLC_EGENERIC;
}
