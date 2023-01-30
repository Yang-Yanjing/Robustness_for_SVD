}
static bool IsWhitespace( const char *psz_string )
{
    psz_string += strspn( psz_string, " \t\r\n" );
    return !*psz_string;
}
