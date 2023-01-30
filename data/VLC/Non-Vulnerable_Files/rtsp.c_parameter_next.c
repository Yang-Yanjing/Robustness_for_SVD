/** Finds the next transport parameter */
static inline const char *parameter_next( const char *str )
{
    while( strchr( ",;", *str ) == NULL )
        str++;
    return (*str == ';') ? (str + 1) : NULL;
}
