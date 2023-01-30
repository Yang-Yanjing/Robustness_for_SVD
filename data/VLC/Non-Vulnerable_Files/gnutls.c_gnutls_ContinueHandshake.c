 */
static int gnutls_ContinueHandshake (vlc_tls_t *session, const char *host,
                                     const char *service)
{
    vlc_tls_sys_t *sys = session->sys;
    int val;
#ifdef _WIN32
    WSASetLastError (0);
#endif
    do
    {
        val = gnutls_handshake (sys->session);
        msg_Dbg (session, "TLS handshake: %s", gnutls_strerror (val));
        if ((val == GNUTLS_E_AGAIN) || (val == GNUTLS_E_INTERRUPTED))
            /* I/O event: return to caller's poll() loop */
            return 1 + gnutls_record_get_direction (sys->session);
    }
    while (val < 0 && !gnutls_error_is_fatal (val));
    if (val < 0)
    {
#ifdef _WIN32
        msg_Dbg (session, "Winsock error %d", WSAGetLastError ());
#endif
        msg_Err (session, "TLS handshake error: %s", gnutls_strerror (val));
        return -1;
    }
    sys->handshaked = true;
    (void) host; (void) service;
    return 0;
}
