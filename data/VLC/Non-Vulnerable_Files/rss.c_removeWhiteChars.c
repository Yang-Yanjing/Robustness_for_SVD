 ***************************************************************************/
static char *removeWhiteChars( const char *psz_src )
{
    char *psz_src2,*psz_clean, *psz_clean2;
    psz_src2 = psz_clean = strdup( psz_src );
    int i;
    while( ( *psz_clean == ' ' || *psz_clean == '\t'
           || *psz_clean == '\n' || *psz_clean == '\r' )
           && *psz_clean != '\0' )
    {
        psz_clean++;
    }
    i = strlen( psz_clean );
    while( --i > 0 &&
         ( psz_clean[i] == ' ' || psz_clean[i] == '\t'
        || psz_clean[i] == '\n' || psz_clean[i] == '\r' ) );
    psz_clean[i+1] = '\0';
    psz_clean2 = strdup( psz_clean );
    free( psz_src2 );
    return psz_clean2;
}
