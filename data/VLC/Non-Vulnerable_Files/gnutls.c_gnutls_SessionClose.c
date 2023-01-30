 */
static void gnutls_SessionClose (vlc_tls_creds_t *crd, vlc_tls_t *session)
{
    vlc_tls_sys_t *sys = session->sys;
    if (sys->handshaked)
        gnutls_bye (sys->session, GNUTLS_SHUT_WR);
    gnutls_deinit (sys->session);
    free (sys);
    (void) crd;
}
