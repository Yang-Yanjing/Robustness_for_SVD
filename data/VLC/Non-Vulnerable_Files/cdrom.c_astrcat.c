/* */
static void astrcat( char **ppsz_dst, char *psz_src )
{
    char *psz_old = *ppsz_dst;
    if( !psz_old )
    {
        *ppsz_dst = strdup( psz_src );
    }
    else if( psz_src )
    {
        if( asprintf( ppsz_dst, "%s%s", psz_old, psz_src ) < 0 )
            *ppsz_dst = psz_old;
        else
            free( psz_old );
    }
}
