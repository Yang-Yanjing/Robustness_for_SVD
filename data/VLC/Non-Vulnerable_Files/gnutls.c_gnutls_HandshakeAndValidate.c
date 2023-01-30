};
static int gnutls_HandshakeAndValidate (vlc_tls_t *session, const char *host,
                                        const char *service)
{
    vlc_tls_sys_t *sys = session->sys;
    int val = gnutls_ContinueHandshake (session, host, service);
    if (val)
        return val;
    /* certificates chain verification */
    unsigned status;
    val = gnutls_certificate_verify_peers2 (sys->session, &status);
    if (val)
    {
        msg_Err (session, "Certificate verification error: %s",
                 gnutls_strerror (val));
        return -1;
    }
    if (status)
    {
        msg_Err (session, "Certificate verification failure (0x%04X)", status);
        for (size_t i = 0; i < sizeof (cert_errs) / sizeof (cert_errs[0]); i++)
            if (status & cert_errs[i].flag)
                msg_Err (session, " * %s", cert_errs[i].msg);
        if (status & ~(GNUTLS_CERT_INVALID|GNUTLS_CERT_SIGNER_NOT_FOUND))
            return -1;
    }
    if (host == NULL)
        return status ? -1 : 0;
    /* certificate (host)name verification */
    const gnutls_datum_t *data;
    unsigned count;
    data = gnutls_certificate_get_peers (sys->session, &count);
    if (data == NULL || count == 0)
    {
        msg_Err (session, "Peer certificate not available");
        return -1;
    }
    msg_Dbg (session, "%u certificate(s) in the list", count);
    if (status && gnutls_CertSearch (session, host, service, data))
        return -1;
    gnutls_x509_crt_t cert;
    val = gnutls_x509_crt_init (&cert);
    if (val)
    {
        msg_Err (session, "X.509 fatal error: %s", gnutls_strerror (val));
        return -1;
    }
    val = gnutls_x509_crt_import (cert, data, GNUTLS_X509_FMT_DER);
    if (val)
    {
        msg_Err (session, "Certificate import error: %s",
                 gnutls_strerror (val));
        goto error;
    }
    val = !gnutls_x509_crt_check_hostname (cert, host);
    if (val)
    {
        msg_Err (session, "Certificate does not match \"%s\"", host);
        val = gnutls_CertSearch (session, host, service, data);
    }
error:
    gnutls_x509_crt_deinit (cert);
    return val;
}
