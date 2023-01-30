 */
static void CloseServer (vlc_tls_creds_t *crd)
{
    vlc_tls_creds_sys_t *sys = crd->sys;
    /* all sessions depending on the server are now deinitialized */
    gnutls_certificate_free_credentials (sys->x509_cred);
    gnutls_dh_params_deinit (sys->dh_params);
    free (sys);
    gnutls_Deinit ();
}
