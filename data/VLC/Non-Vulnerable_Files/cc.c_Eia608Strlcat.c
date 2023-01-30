}
static void Eia608Strlcat( char *d, const char *s, int i_max )
{
    if( i_max > 1 )
        strncat( d, s, i_max-1 - strnlen(d, i_max-1));
    if( i_max > 0 )
        d[i_max-1] = '\0';
}
