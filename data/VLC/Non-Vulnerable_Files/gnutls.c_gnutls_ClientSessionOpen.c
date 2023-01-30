}
static int gnutls_ClientSessionOpen (vlc_tls_creds_t *crd, vlc_tls_t *session,
                                     int fd, const char *hostname)
{
    int val = gnutls_SessionOpen (crd, session, GNUTLS_CLIENT, fd);
    if (val != VLC_SUCCESS)
        return val;
    vlc_tls_sys_t *sys = session->sys;
    /* minimum DH prime bits */
    gnutls_dh_set_prime_bits (sys->session, 1024);
    if (likely(hostname != NULL))
        /* fill Server Name Indication */
        gnutls_server_name_set (sys->session, GNUTLS_NAME_DNS,
                                hostname, strlen (hostname));
    return VLC_SUCCESS;
}
