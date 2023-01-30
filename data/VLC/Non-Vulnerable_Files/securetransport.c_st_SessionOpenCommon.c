 */
static int st_SessionOpenCommon (vlc_tls_creds_t *crd, vlc_tls_t *session,
                                 int fd, bool b_server) {
    vlc_tls_sys_t *sys = malloc(sizeof(*session->sys));
    if (unlikely(sys == NULL))
        return VLC_ENOMEM;
    sys->p_cred = crd->sys;
    sys->i_fd = fd;
    sys->b_handshaked = false;
    sys->b_blocking_send = false;
    sys->i_send_buffered_bytes = 0;
    sys->p_context = NULL;
    session->sys = sys;
    session->sock.p_sys = session;
    session->sock.pf_send = st_Send;
    session->sock.pf_recv = st_Recv;
    session->handshake = st_Handshake;
    SSLContextRef p_context = NULL;
#if TARGET_OS_IPHONE
    p_context = SSLCreateContext(NULL, b_server ? kSSLServerSide : kSSLClientSide, kSSLStreamType);
    if (p_context == NULL) {
        msg_Err(session, "cannot create ssl context");
        return -1;
    }
#else
    if (SSLNewContext(b_server, &p_context) != noErr) {
        msg_Err(session, "error calling SSLNewContext");
        return -1;
    }
#endif
    sys->p_context = p_context;
    OSStatus ret = SSLSetIOFuncs(p_context, st_SocketReadFunc, st_SocketWriteFunc);
    if (ret != noErr) {
        msg_Err(session, "cannot set io functions");
        return -1;
    }
    ret = SSLSetConnection(p_context, session);
    if (ret != noErr) {
        msg_Err(session, "cannot set connection");
        return -1;
    }
    return 0;
}
