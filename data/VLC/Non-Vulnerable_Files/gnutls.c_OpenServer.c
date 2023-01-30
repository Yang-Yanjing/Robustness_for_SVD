 */
static int OpenServer (vlc_tls_creds_t *crd, const char *cert, const char *key)
{
    int val;
    if (gnutls_Init (VLC_OBJECT(crd)))
        return VLC_EGENERIC;
    vlc_tls_creds_sys_t *sys = malloc (sizeof (*sys));
    if (unlikely(sys == NULL))
    {
        gnutls_Deinit ();
        return VLC_ENOMEM;
    }
    crd->sys     = sys;
    crd->add_CA  = gnutls_AddCA;
    crd->add_CRL = gnutls_AddCRL;
    crd->open    = gnutls_ServerSessionOpen;
    crd->close   = gnutls_SessionClose;
    /* No certificate validation by default */
    sys->handshake  = gnutls_ContinueHandshake;
    /* Sets server's credentials */
    val = gnutls_certificate_allocate_credentials (&sys->x509_cred);
    if (val != 0)
    {
        msg_Err (crd, "cannot allocate credentials: %s",
                 gnutls_strerror (val));
        free (sys);
        gnutls_Deinit ();
        return VLC_ENOMEM;
    }
    block_t *certblock = block_FilePath (cert);
    if (certblock == NULL)
    {
        msg_Err (crd, "cannot read certificate chain from %s: %s", cert,
                 vlc_strerror_c(errno));
        goto error;
    }
    block_t *keyblock = block_FilePath (key);
    if (keyblock == NULL)
    {
        msg_Err (crd, "cannot read private key from %s: %s", key,
                 vlc_strerror_c(errno));
        block_Release (certblock);
        goto error;
    }
    gnutls_datum_t pub = {
       .data = certblock->p_buffer,
       .size = certblock->i_buffer,
    }, priv = {
       .data = keyblock->p_buffer,
       .size = keyblock->i_buffer,
    };
    val = gnutls_certificate_set_x509_key_mem (sys->x509_cred, &pub, &priv,
                                                GNUTLS_X509_FMT_PEM);
    block_Release (keyblock);
    block_Release (certblock);
    if (val < 0)
    {
        msg_Err (crd, "cannot load X.509 key: %s", gnutls_strerror (val));
        goto error;
    }
    /* FIXME:
     * - regenerate these regularly
     * - support other cipher suites
     */
    val = gnutls_dh_params_init (&sys->dh_params);
    if (val >= 0)
    {
        gnutls_sec_param_t sec = GNUTLS_SEC_PARAM_MEDIUM;
        unsigned bits = gnutls_sec_param_to_pk_bits (GNUTLS_PK_DH, sec);
        msg_Dbg (crd, "generating Diffie-Hellman %u-bits parameters...", bits);
        val = gnutls_dh_params_generate2 (sys->dh_params, bits);
        if (val == 0)
            gnutls_certificate_set_dh_params (sys->x509_cred,
                                              sys->dh_params);
    }
    if (val < 0)
    {
        msg_Err (crd, "cannot initialize DHE cipher suites: %s",
                 gnutls_strerror (val));
    }
    msg_Dbg (crd, "ciphers parameters loaded");
    return VLC_SUCCESS;
error:
    gnutls_certificate_free_credentials (sys->x509_cred);
    free (sys);
    gnutls_Deinit ();
    return VLC_EGENERIC;
}
