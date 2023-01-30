 */
static int gnutls_AddCA (vlc_tls_creds_t *crd, const char *path)
{
    block_t *block = block_FilePath (path);
    if (block == NULL)
    {
        msg_Err (crd, "cannot read trusted CA from %s: %s", path,
                 vlc_strerror_c(errno));
        return VLC_EGENERIC;
    }
    gnutls_datum_t d = {
       .data = block->p_buffer,
       .size = block->i_buffer,
    };
    int val = gnutls_certificate_set_x509_trust_mem (crd->sys->x509_cred, &d,
                                                     GNUTLS_X509_FMT_PEM);
    block_Release (block);
    if (val < 0)
    {
        msg_Err (crd, "cannot load trusted CA from %s: %s", path,
                 gnutls_strerror (val));
        return VLC_EGENERIC;
    }
    msg_Dbg (crd, " %d trusted CA%s added from %s", val, (val != 1) ? "s" : "",
             path);
    /* enables peer's certificate verification */
    crd->sys->handshake = gnutls_HandshakeAndValidate;
    return VLC_SUCCESS;
}
