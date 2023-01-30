 */
static int ParseTime( const char *s, size_t i_strlen)
{
    // need to parse hour:minutes:sec.fraction string
    int result = 0;
    int val;
    const char *end = s + i_strlen;
    // skip leading spaces if any
    s = SkipBlanks(s, i_strlen);
    val = 0;
    while( (s < end) && isdigit((unsigned char)*s) )
    {
        int newval = val*10 + (*s - '0');
        if( newval < val )
        {
            // overflow
            val = 0;
            break;
        }
        val = newval;
        ++s;
    }
    result = val;
    s = SkipBlanks(s, end-s);
    if( *s == ':' )
    {
        ++s;
        s = SkipBlanks(s, end-s);
        result = result * 60;
        val = 0;
        while( (s < end) && isdigit((unsigned char)*s) )
        {
            int newval = val*10 + (*s - '0');
            if( newval < val )
            {
                // overflow
                val = 0;
                break;
            }
            val = newval;
            ++s;
        }
        result += val;
        s = SkipBlanks(s, end-s);
        if( *s == ':' )
        {
            ++s;
            s = SkipBlanks(s, end-s);
            result = result * 60;
            val = 0;
            while( (s < end) && isdigit((unsigned char)*s) )
            {
                int newval = val*10 + (*s - '0');
                if( newval < val )
                {
                    // overflow
                    val = 0;
                    break;
                }
                val = newval;
                ++s;
            }
            result += val;
            // TODO: one day, we may need to parse fraction for sub-second resolution
        }
    }
    return result;
}
