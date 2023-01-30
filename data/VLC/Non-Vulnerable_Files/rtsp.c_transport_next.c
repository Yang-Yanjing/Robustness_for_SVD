/** Finds the next transport choice */
static inline const char *transport_next( const char *str )
{
    /* Looks for comma */
    str = strchr( str, ',' );
    if( str == NULL )
        return NULL; /* No more transport options */
    str++; /* skips comma */
    while( strchr( "\r\n\t ", *str ) )
        str++;
    return (*str) ? str : NULL;
}
