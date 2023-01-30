};
static char * es_print_url( const char *psz_fmt, vlc_fourcc_t i_fourcc, int i_count,
                            const char *psz_access, const char *psz_mux )
{
    char *psz_dst, *p;
    if( psz_fmt == NULL || !*psz_fmt )
    {
        psz_fmt = (char*)"stream-%n-%c.%m";
    }
    p = psz_dst = malloc( 4096 );
    if( !psz_dst )
        return NULL;
    memset( p, 0, 4096 );
    for( ;; )
    {
        if( *psz_fmt == '\0' )
        {
            *p = '\0';
            break;
        }
        if( *psz_fmt != '%' )
        {
            *p++ = *psz_fmt++;
        }
        else
        {
            if( psz_fmt[1] == 'n' )
            {
                p += sprintf( p, "%d", i_count );
            }
            else if( psz_fmt[1] == 'c' )
            {
                p += sprintf( p, "%4.4s", (char*)&i_fourcc );
            }
            else if( psz_fmt[1] == 'm' )
            {
                p += sprintf( p, "%s", psz_mux );
            }
            else if( psz_fmt[1] == 'a' )
            {
                p += sprintf( p, "%s", psz_access );
            }
            else if( psz_fmt[1] != '\0' )
            {
                p += sprintf( p, "%c%c", psz_fmt[0], psz_fmt[1] );
            }
            else
            {
                p += sprintf( p, "%c", psz_fmt[0] );
                *p++ = '\0';
                break;
            }
            psz_fmt += 2;
        }
    }
    return( psz_dst );
}
