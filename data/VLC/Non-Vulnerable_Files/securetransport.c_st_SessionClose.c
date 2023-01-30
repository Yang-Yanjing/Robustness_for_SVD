 */
static void st_SessionClose (vlc_tls_creds_t *crd, vlc_tls_t *session) {
    VLC_UNUSED(crd);
    vlc_tls_sys_t *sys = session->sys;
    msg_Dbg(session, "close TLS session");
    if (sys->p_context) {
        if (sys->b_handshaked) {
            OSStatus ret = SSLClose(sys->p_context);
            if (ret != noErr) {
                msg_Warn(session, "Cannot close ssl context");
            }
        }
#if TARGET_OS_IPHONE
        CFRelease(sys->p_context);
#else
        if (SSLDisposeContext(sys->p_context) != noErr) {
            msg_Err(session, "error deleting context");
        }
#endif
    }
    free(sys);
}
