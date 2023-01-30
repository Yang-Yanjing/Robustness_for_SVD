static int   Control( stream_t *, int , va_list );
static bool isSmoothStreaming( stream_t *s )
{
    const uint8_t *peek;
    int i_size = stream_Peek( s->p_source, &peek, 512 );
    if( i_size < 512 )
        return false;
    char *peeked = malloc( 512 );
    if( unlikely( peeked == NULL ) )
        return false;
    memcpy( peeked, peek, 512 );
    peeked[511] = peeked[510] = '\0';
    char *str;
    if( !memcmp( peeked, "\xFF\xFE", 2 ) )
    {
        str = FromCharset( "UTF-16LE", peeked, 512 );
        free( peeked );
    }
    else if( !memcmp( peeked, "\xFE\xFF", 2 ) )
    {
        str = FromCharset( "UTF-16BE", peeked, 512 );
        free( peeked );
    }
    else
        str = peeked;
    if( str == NULL )
        return false;
    bool ret = strstr( str, "<SmoothStreamingMedia" ) != NULL;
    free( str );
    return ret;
}
