 *****************************************************************************/
void input_SplitMRL( const char **access, const char **demux,
                     const char **path, const char **anchor, char *buf )
{
    char *p;
    /* Separate <path> from <access>[/<demux>]:// */
    p = strstr( buf, "://" );
    if( p != NULL )
    {
        *p = '\0';
        p += 3; /* skips "://" */
        *path = p;
        /* Remove HTML anchor if present (not supported).
         * The hash symbol itself should be URI-encoded. */
        p = strchr( p, '#' );
        if( p != NULL )
        {
            *(p++) = '\0';
            *anchor = p;
        }
        else
            *anchor = "";
    }
    else
    {
#ifndef NDEBUG
        fprintf( stderr, "%s(\"%s\") probably not a valid URI!\n", __func__,
                 buf );
#endif
        /* Note: this is a valid non const pointer to "": */
        *path = buf + strlen( buf );
    }
    /* Separate access from demux */
    p = strchr( buf, '/' );
    if( p != NULL )
    {
        *(p++) = '\0';
        if( p[0] == '$' )
            p++;
        *demux = p;
    }
    else
        *demux = "";
    /* We really don't want module name substitution here! */
    p = buf;
    if( p[0] == '$' )
        p++;
    *access = p;
}
