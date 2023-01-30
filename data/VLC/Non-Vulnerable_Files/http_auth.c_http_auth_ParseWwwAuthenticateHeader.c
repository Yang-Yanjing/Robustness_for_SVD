 */
void http_auth_ParseWwwAuthenticateHeader(
        vlc_object_t *p_this, http_auth_t *p_auth,
        const char *psz_header )
{
    static const char psz_basic_prefix[] = "Basic ";
    static const char psz_digest_prefix[] = "Digest ";
    /* FIXME: multiple auth methods can be listed (comma separated) */
    if ( strncasecmp( psz_header, psz_basic_prefix,
                      sizeof( psz_basic_prefix ) - 1 ) == 0 )
    {
        /* 2 Basic Authentication Scheme */
        msg_Dbg( p_this, "Using Basic Authentication" );
        psz_header += sizeof( psz_basic_prefix ) - 1;
        p_auth->psz_realm = AuthGetParam( psz_header, "realm" );
        if ( p_auth->psz_realm == NULL )
            msg_Warn( p_this, "Basic Authentication: "
                      "Mandatory 'realm' parameter is missing" );
    }
    else if ( strncasecmp( psz_header, psz_digest_prefix,
                           sizeof( psz_digest_prefix ) - 1 ) == 0 )
    {
        /* 3 Digest Access Authentication Scheme */
        msg_Dbg( p_this, "Using Digest Access Authentication" );
        if ( p_auth->psz_nonce )
            /* FIXME */
            return;
        psz_header += sizeof( psz_digest_prefix ) - 1;
        p_auth->psz_realm = AuthGetParam( psz_header, "realm" );
        p_auth->psz_domain = AuthGetParam( psz_header, "domain" );
        p_auth->psz_nonce = AuthGetParam( psz_header, "nonce" );
        p_auth->psz_opaque = AuthGetParam( psz_header, "opaque" );
        p_auth->psz_stale = AuthGetParamNoQuotes( psz_header, "stale" );
        p_auth->psz_algorithm = AuthGetParamNoQuotes( psz_header, "algorithm" );
        p_auth->psz_qop = AuthGetParam( psz_header, "qop" );
        p_auth->i_nonce = 0;
        /* printf("realm: |%s|\ndomain: |%s|\nnonce: |%s|\nopaque: |%s|\n"
                  "stale: |%s|\nalgorithm: |%s|\nqop: |%s|\n",
                  p_auth->psz_realm,p_auth->psz_domain,p_auth->psz_nonce,
                  p_auth->psz_opaque,p_auth->psz_stale,p_auth->psz_algorithm,
                  p_auth->psz_qop); */
        if ( p_auth->psz_realm == NULL )
            msg_Warn( p_this, "Digest Access Authentication: "
                      "Mandatory 'realm' parameter is missing" );
        if ( p_auth->psz_nonce == NULL )
            msg_Warn( p_this, "Digest Access Authentication: "
                      "Mandatory 'nonce' parameter is missing" );
        /* FIXME: parse the qop list */
        if ( p_auth->psz_qop )
        {
            char *psz_tmp = strchr( p_auth->psz_qop, ',' );
            if ( psz_tmp )
                *psz_tmp = '\0';
        }
    }
    else
    {
        const char *psz_end = strchr( psz_header, ' ' );
        if ( psz_end )
            msg_Warn( p_this, "Unknown authentication scheme: '%*s'",
                      (int)(psz_end - psz_header), psz_header );
        else
            msg_Warn( p_this, "Unknown authentication scheme: '%s'",
                      psz_header );
    }
}
