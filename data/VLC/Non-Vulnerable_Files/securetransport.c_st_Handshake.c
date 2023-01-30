 */
static int st_Handshake (vlc_tls_t *session, const char *host,
                                        const char *service) {
    VLC_UNUSED(service);
    vlc_tls_sys_t *sys = session->sys;
    OSStatus retValue = SSLHandshake(sys->p_context);
    if (retValue == errSSLWouldBlock) {
        msg_Dbg(session, "handshake is blocked, try again later");
        return 1 + (sys->b_blocking_send ? 1 : 0);
    }
    switch (retValue) {
        case noErr:
            if (sys->b_server_mode == false && st_validateServerCertificate(session, host) != 0) {
                return -1;
            }
            msg_Dbg(session, "handshake completed successfully");
            sys->b_handshaked = true;
            return 0;
        case errSSLServerAuthCompleted:
            return st_Handshake(session, host, service);
        case errSSLConnectionRefused:
            msg_Err(session, "connection was refused");
            return -1;
        case errSSLNegotiation:
            msg_Err(session, "cipher suite negotiation failed");
            return -1;
        case errSSLFatalAlert:
            msg_Err(session, "fatal error occured during handshake");
            return -1;
        default:
            msg_Err(session, "handshake returned error %d", (int)retValue);
            return -1;
    }
}
