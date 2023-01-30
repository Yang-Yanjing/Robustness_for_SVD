 */
static const char *SkipBlanks( const char *s, size_t i_strlen )
{
    while( i_strlen > 0 ) {
        switch( *s )
        {
            case ' ':
            case '\t':
            case '\r':
            case '\n':
                --i_strlen;
                ++s;
                break;
            default:
                i_strlen = 0;
        }
    }
    return s;
}
