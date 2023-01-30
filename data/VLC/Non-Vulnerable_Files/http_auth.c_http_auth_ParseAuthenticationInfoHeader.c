 */
int http_auth_ParseAuthenticationInfoHeader(
        vlc_object_t *p_this, http_auth_t *p_auth,
        const char *psz_header, const char *psz_method, const char *psz_path,
        const char *psz_username, const char *psz_password )
{
    char *psz_nextnonce = AuthGetParam( psz_header, "nextnonce" );
    char *psz_qop = AuthGetParamNoQuotes( psz_header, "qop" );
    char *psz_rspauth = AuthGetParam( psz_header, "rspauth" );
    char *psz_cnonce = AuthGetParam( psz_header, "cnonce" );
    char *psz_nc = AuthGetParamNoQuotes( psz_header, "nc" );
    char *psz_digest = NULL;
    int i_err = VLC_SUCCESS;
    int i_nonce;
    if ( psz_cnonce )
    {
        if ( strcmp( psz_cnonce, p_auth->psz_cnonce ) != 0 )
        {
            msg_Err( p_this, "HTTP Digest Access Authentication: server "
                             "replied with a different client nonce value." );
            i_err = VLC_EGENERIC;
            goto error;
        }
        if ( psz_nc )
        {
            i_nonce = strtol( psz_nc, NULL, 16 );
            if ( i_nonce != p_auth->i_nonce )
            {
                msg_Err( p_this, "HTTP Digest Access Authentication: server "
                                 "replied with a different nonce count "
                                 "value." );
                i_err = VLC_EGENERIC;
                goto error;
            }
        }
        if ( psz_qop && p_auth->psz_qop &&
             strcmp( psz_qop, p_auth->psz_qop ) != 0 )
            msg_Warn( p_this, "HTTP Digest Access Authentication: server "
                              "replied using a different 'quality of "
                              "protection' option" );
        /* All the clear text values match, let's now check the response
         * digest.
         *
         * TODO: Support for "qop=auth-int"
         */
        psz_digest = AuthDigest( p_this, p_auth, psz_method, psz_path,
                                 psz_username, psz_password );
        if ( strcmp( psz_digest, psz_rspauth ) != 0 )
        {
            msg_Err( p_this, "HTTP Digest Access Authentication: server "
                             "replied with an invalid response digest "
                             "(expected value: %s).", psz_digest );
            i_err = VLC_EGENERIC;
            goto error;
        }
    }
    if ( psz_nextnonce )
    {
        free( p_auth->psz_nonce );
        p_auth->psz_nonce = psz_nextnonce;
        psz_nextnonce = NULL;
    }
error:
    free( psz_nextnonce );
    free( psz_qop );
    free( psz_rspauth );
    free( psz_cnonce );
    free( psz_nc );
    free( psz_digest );
    return i_err;
}
