}
static void CloseClient (vlc_tls_creds_t *crd)
{
    vlc_tls_creds_sys_t *sys = crd->sys;
    gnutls_certificate_free_credentials (sys->x509_cred);
    free (sys);
    gnutls_Deinit ();
}
