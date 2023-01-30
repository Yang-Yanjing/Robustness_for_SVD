 */
static int OpenClient (vlc_tls_creds_t *crd)
{
    if (gnutls_Init (VLC_OBJECT(crd)))
        return VLC_EGENERIC;
    vlc_tls_creds_sys_t *sys = malloc (sizeof (*sys));
    if (unlikely(sys == NULL))
        goto error;
    crd->sys = sys;
    //crd->add_CA = gnutls_AddCA;
    //crd->add_CRL = gnutls_AddCRL;
    crd->open = gnutls_ClientSessionOpen;
    crd->close = gnutls_SessionClose;
    sys->handshake = gnutls_HandshakeAndValidate;
    int val = gnutls_certificate_allocate_credentials (&sys->x509_cred);
    if (val != 0)
    {
        msg_Err (crd, "cannot allocate credentials: %s",
                 gnutls_strerror (val));
        goto error;
    }
    val = gnutls_certificate_set_x509_system_trust (sys->x509_cred);
    if (val < 0)
        msg_Err (crd, "cannot load trusted Certificate Authorities: %s",
                 gnutls_strerror (val));
    else
        msg_Dbg (crd, "loaded %d trusted CAs", val);
    gnutls_certificate_set_verify_flags (sys->x509_cred,
                                         GNUTLS_VERIFY_ALLOW_X509_V1_CA_CRT);
    return VLC_SUCCESS;
error:
    free (sys);
    gnutls_Deinit ();
    return VLC_EGENERIC;
}
