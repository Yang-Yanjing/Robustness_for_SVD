}
static int gnutls_ServerSessionOpen (vlc_tls_creds_t *crd, vlc_tls_t *session,
                                     int fd, const char *hostname)
{
    int val = gnutls_SessionOpen (crd, session, GNUTLS_SERVER, fd);
    if (val != VLC_SUCCESS)
        return val;
    if (session->handshake == gnutls_HandshakeAndValidate)
        gnutls_certificate_server_set_request (session->sys->session,
                                               GNUTLS_CERT_REQUIRE);
    assert (hostname == NULL);
    return VLC_SUCCESS;
}
