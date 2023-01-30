}
void http_auth_Reset( http_auth_t *p_auth )
{
    p_auth->i_nonce = 0;
    FREENULL( p_auth->psz_realm );
    FREENULL( p_auth->psz_domain );
    FREENULL( p_auth->psz_nonce );
    FREENULL( p_auth->psz_opaque );
    FREENULL( p_auth->psz_stale );
    FREENULL( p_auth->psz_algorithm );
    FREENULL( p_auth->psz_qop );
    FREENULL( p_auth->psz_cnonce );
    FREENULL( p_auth->psz_HA1 );
}
