}
static char *AuthDigest( vlc_object_t *p_this, http_auth_t *p_auth,
                         const char *psz_method, const char *psz_path,
                         const char *psz_username, const char *psz_password )
{
    char *psz_HA1 = NULL;
    char *psz_HA2 = NULL;
    char *psz_ent = NULL;
    char *psz_result = NULL;
    char psz_inonce[9];
    struct md5_s md5;
    struct md5_s ent;
    if ( p_auth->psz_realm == NULL )
    {
        msg_Warn( p_this, "Digest Authentication: "
                  "Mandatory 'realm' value not available" );
        goto error;
    }
    /* H(A1) */
    if ( p_auth->psz_HA1 )
    {
        psz_HA1 = strdup( p_auth->psz_HA1 );
        if ( psz_HA1 == NULL )
            goto error;
    }
    else
    {
        InitMD5( &md5 );
        AddMD5( &md5, psz_username, strlen( psz_username ) );
        AddMD5( &md5, ":", 1 );
        AddMD5( &md5, p_auth->psz_realm, strlen( p_auth->psz_realm ) );
        AddMD5( &md5, ":", 1 );
        AddMD5( &md5, psz_password, strlen( psz_password ) );
        EndMD5( &md5 );
        psz_HA1 = psz_md5_hash( &md5 );
        if ( psz_HA1 == NULL )
            goto error;
        if ( p_auth->psz_algorithm &&
             strcmp( p_auth->psz_algorithm, "MD5-sess" ) == 0 )
        {
            InitMD5( &md5 );
            AddMD5( &md5, psz_HA1, 32 );
            AddMD5( &md5, ":", 1 );
            AddMD5( &md5, p_auth->psz_nonce, strlen( p_auth->psz_nonce ) );
            AddMD5( &md5, ":", 1 );
            AddMD5( &md5, p_auth->psz_cnonce, strlen( p_auth->psz_cnonce ) );
            EndMD5( &md5 );
            free( psz_HA1 );
            psz_HA1 = psz_md5_hash( &md5 );
            if ( psz_HA1 == NULL )
                goto error;
            p_auth->psz_HA1 = strdup( psz_HA1 );
            if ( p_auth->psz_HA1 == NULL )
                goto error;
        }
    }
    /* H(A2) */
    InitMD5( &md5 );
    if ( *psz_method )
        AddMD5( &md5, psz_method, strlen( psz_method ) );
    AddMD5( &md5, ":", 1 );
    if ( psz_path )
        AddMD5( &md5, psz_path, strlen( psz_path ) );
    else
        AddMD5( &md5, "/", 1 );
    if ( p_auth->psz_qop && strcmp( p_auth->psz_qop, "auth-int" ) == 0 )
    {
        InitMD5( &ent );
        /* TODO: Support for "qop=auth-int" */
        AddMD5( &ent, "", 0 );
        EndMD5( &ent );
        psz_ent = psz_md5_hash( &ent );
        if ( psz_ent == NULL )
            goto error;
        AddMD5( &md5, ":", 1 );
        AddMD5( &md5, psz_ent, 32 );
    }
    EndMD5( &md5 );
    psz_HA2 = psz_md5_hash( &md5 );
    if ( psz_HA2 == NULL )
        goto error;
    /* Request digest */
    InitMD5( &md5 );
    AddMD5( &md5, psz_HA1, 32 );
    AddMD5( &md5, ":", 1 );
    AddMD5( &md5, p_auth->psz_nonce, strlen( p_auth->psz_nonce ) );
    AddMD5( &md5, ":", 1 );
    if ( p_auth->psz_qop &&
         ( strcmp( p_auth->psz_qop, "auth" ) == 0 ||
           strcmp( p_auth->psz_qop, "auth-int" ) == 0 ) )
    {
        snprintf( psz_inonce, sizeof( psz_inonce ), "%08x", p_auth->i_nonce );
        AddMD5( &md5, psz_inonce, 8 );
        AddMD5( &md5, ":", 1 );
        AddMD5( &md5, p_auth->psz_cnonce, strlen( p_auth->psz_cnonce ) );
        AddMD5( &md5, ":", 1 );
        AddMD5( &md5, p_auth->psz_qop, strlen( p_auth->psz_qop ) );
        AddMD5( &md5, ":", 1 );
    }
    AddMD5( &md5, psz_HA2, 32 );
    EndMD5( &md5 );
    psz_result = psz_md5_hash( &md5 );
error:
    free( psz_HA1 );
    free( psz_HA2 );
    free( psz_ent );
    return psz_result;
}
