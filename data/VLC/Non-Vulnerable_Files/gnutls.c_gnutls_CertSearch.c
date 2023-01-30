 */
static int gnutls_CertSearch (vlc_tls_t *obj, const char *host,
                              const char *service,
                              const gnutls_datum_t *restrict datum)
{
    assert (host != NULL);
    /* Look up mismatching certificate in store */
    int val = gnutls_verify_stored_pubkey (NULL, NULL, host, service,
                                           GNUTLS_CRT_X509, datum, 0);
    const char *msg;
    switch (val)
    {
        case 0:
            msg_Dbg (obj, "certificate key match for %s", host);
            return 0;
        case GNUTLS_E_NO_CERTIFICATE_FOUND:
            msg_Dbg (obj, "no known certificates for %s", host);
            msg = N_("You attempted to reach %s. "
                "However the security certificate presented by the server "
                "is unknown and could not be authenticated by any trusted "
                "Certification Authority. "
                "This problem may be caused by a configuration error "
                "or an attempt to breach your security or your privacy.\n\n"
                "If in doubt, abort now.\n");
            break;
        case GNUTLS_E_CERTIFICATE_KEY_MISMATCH:
            msg_Dbg (obj, "certificate keys mismatch for %s", host);
            msg = N_("You attempted to reach %s. "
                "However the security certificate presented by the server "
                "changed since the previous visit "
                "and was not authenticated by any trusted "
                "Certification Authority. "
                "This problem may be caused by a configuration error "
                "or an attempt to breach your security or your privacy.\n\n"
                "If in doubt, abort now.\n");
            break;
        default:
            msg_Err (obj, "certificate key match error for %s: %s", host,
                     gnutls_strerror (val));
            return -1;
    }
    if (dialog_Question (obj, _("Insecure site"), vlc_gettext (msg),
                         _("Abort"), _("View certificate"), NULL, host) != 2)
        return -1;
    gnutls_x509_crt_t cert;
    gnutls_datum_t desc;
    if (gnutls_x509_crt_init (&cert))
        return -1;
    if (gnutls_x509_crt_import (cert, datum, GNUTLS_X509_FMT_DER)
     || gnutls_x509_crt_print (cert, GNUTLS_CRT_PRINT_ONELINE, &desc))
    {
        gnutls_x509_crt_deinit (cert);
        return -1;
    }
    gnutls_x509_crt_deinit (cert);
    val = dialog_Question (obj, _("Insecure site"),
         _("This is the certificate presented by %s:\n%s\n\n"
           "If in doubt, abort now.\n"),
                           _("Abort"), _("Accept 24 hours"),
                           _("Accept permanently"), host, desc.data);
    gnutls_free (desc.data);
    time_t expiry = 0;
    switch (val)
    {
        case 2:
            time (&expiry);
            expiry += 24 * 60 * 60;
        case 3:
            val = gnutls_store_pubkey (NULL, NULL, host, service,
                                       GNUTLS_CRT_X509, datum, expiry, 0);
            if (val)
                msg_Err (obj, "cannot store X.509 certificate: %s",
                         gnutls_strerror (val));
            return 0;
    }
    return -1;
}
