 *****************************************************************************/
static char *AuthGetParam( const char *psz_header, const char *psz_param )
{
    char psz_what[strlen(psz_param)+3];
    sprintf( psz_what, "%s=\"", psz_param );
    psz_header = strstr( psz_header, psz_what );
    if ( psz_header )
    {
        const char *psz_end;
        psz_header += strlen( psz_what );
        psz_end = strchr( psz_header, '"' );
        if ( !psz_end ) /* Invalid since we should have a closing quote */
            return strdup( psz_header );
        return strndup( psz_header, psz_end - psz_header );
    }
    else
    {
        return NULL;
    }
}
