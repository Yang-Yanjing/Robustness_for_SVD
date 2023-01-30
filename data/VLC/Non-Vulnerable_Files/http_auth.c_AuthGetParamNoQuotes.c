}
static char *AuthGetParamNoQuotes( const char *psz_header, const char *psz_param )
{
    char psz_what[strlen(psz_param)+2];
    sprintf( psz_what, "%s=", psz_param );
    psz_header = strstr( psz_header, psz_what );
    if ( psz_header )
    {
        const char *psz_end;
        psz_header += strlen( psz_what );
        psz_end = strchr( psz_header, ',' );
        /* XXX: Do we need to filter out trailing space between the value and
         * the comma/end of line? */
        if ( !psz_end ) /* Can be valid if this is the last parameter */
            return strdup( psz_header );
        return strndup( psz_header, psz_end - psz_header );
    }
    else
    {
        return NULL;
    }
}
