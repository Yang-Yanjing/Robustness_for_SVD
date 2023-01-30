 */
static int SplitHeader( char **ppsz_next, char **ppsz_name,
                        char **ppsz_value )
{
    /* Find semicolon (separator between assignments) */
    *ppsz_name = strsep( ppsz_next, psz_delim_semicolon );
    if ( *ppsz_name )
    {
        /* Skip spaces */
        *ppsz_name += strspn( *ppsz_name, psz_delim_space );
        /* Get value */
        *ppsz_value = *ppsz_name;
        strsep( ppsz_value, psz_delim_equal );
    }
    else
        *ppsz_value = NULL;
    return !!*ppsz_name;
}
