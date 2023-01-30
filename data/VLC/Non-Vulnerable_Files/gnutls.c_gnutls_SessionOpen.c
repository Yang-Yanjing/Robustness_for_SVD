 */
static int gnutls_SessionOpen (vlc_tls_creds_t *crd, vlc_tls_t *session,
                               int type, int fd)
{
    vlc_tls_sys_t *sys = malloc (sizeof (*session->sys));
    if (unlikely(sys == NULL))
        return VLC_ENOMEM;
    session->sys = sys;
    session->sock.p_sys = session;
    session->sock.pf_send = gnutls_Send;
    session->sock.pf_recv = gnutls_Recv;
    session->handshake = crd->sys->handshake;
    sys->handshaked = false;
    int val = gnutls_init (&sys->session, type);
    if (val != 0)
    {
        msg_Err (session, "cannot initialize TLS session: %s",
                 gnutls_strerror (val));
        free (sys);
        return VLC_EGENERIC;
    }
    if (gnutls_SessionPrioritize (VLC_OBJECT (crd), sys->session))
        goto error;
    val = gnutls_credentials_set (sys->session, GNUTLS_CRD_CERTIFICATE,
                                  crd->sys->x509_cred);
    if (val < 0)
    {
        msg_Err (session, "cannot set TLS session credentials: %s",
                 gnutls_strerror (val));
        goto error;
    }
    gnutls_transport_set_ptr (sys->session,
                              (gnutls_transport_ptr_t)(intptr_t)fd);
    return VLC_SUCCESS;
error:
    gnutls_SessionClose (crd, session);
    return VLC_EGENERIC;
}
