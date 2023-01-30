}
char *http_auth_FormatAuthorizationHeader(
        vlc_object_t *p_this, http_auth_t *p_auth,
        const char *psz_method, const char *psz_path,
        const char *psz_username, const char *psz_password )
{
    char *psz_result = NULL;
    char *psz_buffer = NULL;
    char *psz_base64 = NULL;
    int i_rc;
    if ( p_auth->psz_nonce )
    {
        /* Digest Access Authentication */
        if ( p_auth->psz_algorithm &&
             strcmp( p_auth->psz_algorithm, "MD5" ) != 0 &&
             strcmp( p_auth->psz_algorithm, "MD5-sess" ) != 0 )
        {
            msg_Err( p_this, "Digest Access Authentication: "
                     "Unknown algorithm '%s'", p_auth->psz_algorithm );
            goto error;
        }
        if ( p_auth->psz_qop != NULL || p_auth->psz_cnonce == NULL )
        {
            free( p_auth->psz_cnonce );
            p_auth->psz_cnonce = GenerateCnonce();
            if ( p_auth->psz_cnonce == NULL )
                goto error;
        }
        ++p_auth->i_nonce;
        psz_buffer = AuthDigest( p_this, p_auth, psz_method, psz_path,
                                 psz_username, psz_password );
        if ( psz_buffer == NULL )
            goto error;
        i_rc = asprintf( &psz_result,
            "Digest "
            /* Mandatory parameters */
            "username=\"%s\", "
            "realm=\"%s\", "
            "nonce=\"%s\", "
            "uri=\"%s\", "
            "response=\"%s\", "
            /* Optional parameters */
            "%s%s%s" /* algorithm */
            "%s%s%s" /* cnonce */
            "%s%s%s" /* opaque */
            "%s%s%s" /* message qop */
            "%s%08x%s", /* nonce count */
            /* Mandatory parameters */
            psz_username,
            p_auth->psz_realm,
            p_auth->psz_nonce,
            psz_path ? psz_path : "/",
            psz_buffer,
            /* Optional parameters */
            p_auth->psz_algorithm ? "algorithm=\"" : "",
            p_auth->psz_algorithm ? p_auth->psz_algorithm : "",
            p_auth->psz_algorithm ? "\", " : "",
            p_auth->psz_cnonce ? "cnonce=\"" : "",
            p_auth->psz_cnonce ? p_auth->psz_cnonce : "",
            p_auth->psz_cnonce ? "\", " : "",
            p_auth->psz_opaque ? "opaque=\"" : "",
            p_auth->psz_opaque ? p_auth->psz_opaque : "",
            p_auth->psz_opaque ? "\", " : "",
            p_auth->psz_qop ? "qop=\"" : "",
            p_auth->psz_qop ? p_auth->psz_qop : "",
            p_auth->psz_qop ? "\", " : "",
            /* "uglyhack" will be parsed as an unhandled extension */
            p_auth->i_nonce ? "nc=\"" : "uglyhack=\"",
            p_auth->i_nonce,
            p_auth->i_nonce ? "\"" : "\""
        );
        if ( i_rc < 0 )
            goto error;
    }
    else
    {
        /* Basic Access Authentication */
        i_rc = asprintf( &psz_buffer, "%s:%s", psz_username, psz_password );
        if ( i_rc < 0 )
            goto error;
        psz_base64 = vlc_b64_encode( psz_buffer );
        if ( psz_base64 == NULL )
            goto error;
        i_rc = asprintf( &psz_result, "Basic %s", psz_base64 );
        if ( i_rc < 0 )
            goto error;
    }
error:
    free( psz_buffer );
    free( psz_base64 );
    return psz_result;
}
