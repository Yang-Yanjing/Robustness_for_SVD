}
static const char *IsASCII( const char *str )
{
    int8_t c;
    for( const char *p = str; (c = *p) != '\0'; p++ )
        if( c < 0 )
            return NULL;
    return str;
}
