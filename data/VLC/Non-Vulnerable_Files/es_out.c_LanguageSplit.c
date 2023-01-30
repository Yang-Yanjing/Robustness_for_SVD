}
static char **LanguageSplit( const char *psz_langs, bool b_default_any )
{
    char *psz_dup;
    char *psz_parser;
    char **ppsz = NULL;
    int i_psz = 0;
    if( psz_langs == NULL ) return NULL;
    psz_parser = psz_dup = strdup(psz_langs);
    while( psz_parser && *psz_parser )
    {
        char *psz;
        char *psz_code;
        psz = strchr(psz_parser, ',' );
        if( psz ) *psz++ = '\0';
        if( !strcmp( psz_parser, "any" ) )
        {
            TAB_APPEND( i_psz, ppsz, strdup("any") );
        }
        else if( !strcmp( psz_parser, "none" ) )
        {
            TAB_APPEND( i_psz, ppsz, strdup("none") );
        }
        else
        {
            psz_code = LanguageGetCode( psz_parser );
            if( strcmp( psz_code, "??" ) )
            {
                TAB_APPEND( i_psz, ppsz, psz_code );
            }
            else
            {
                free( psz_code );
            }
        }
        psz_parser = psz;
    }
    if( i_psz )
    {
        if( b_default_any && strcmp( ppsz[i_psz - 1], "none" ) )
            TAB_APPEND( i_psz, ppsz, strdup("any") );
        TAB_APPEND( i_psz, ppsz, NULL );
    }
    free( psz_dup );
    return ppsz;
}
