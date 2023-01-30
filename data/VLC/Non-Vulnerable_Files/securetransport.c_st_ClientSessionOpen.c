}
static int st_ClientSessionOpen (vlc_tls_creds_t *crd, vlc_tls_t *session,
                                     int fd, const char *hostname) {
    msg_Dbg(session, "open TLS session for %s", hostname);
    int ret = st_SessionOpenCommon(crd, session, fd, false);
    if (ret != noErr) {
        goto error;
    }
    vlc_tls_sys_t *sys = session->sys;
    sys->b_server_mode = false;
    ret = SSLSetPeerDomainName(sys->p_context, hostname, strlen(hostname));
    if (ret != noErr) {
        msg_Err(session, "cannot set peer domain name");
        goto error;
    }
    /* disable automatic validation. We do so manually to also handle invalid
       certificates */
    /* this has effect only on iOS 5 and OSX 10.8 or later ... */
    ret = SSLSetSessionOption(sys->p_context, kSSLSessionOptionBreakOnServerAuth, true);
    if (ret != noErr) {
        msg_Err (session, "cannot set session option");
        goto error;
    }
#if !TARGET_OS_IPHONE
    /* ... thus calling this for earlier osx versions, which is not available on iOS in turn */
    ret = SSLSetEnableCertVerify(sys->p_context, false);
    if (ret != noErr) {
        msg_Err(session, "error setting enable cert verify");
        goto error;
    }
#endif
    return VLC_SUCCESS;
error:
    st_SessionClose(crd, session);
    return VLC_EGENERIC;
}
