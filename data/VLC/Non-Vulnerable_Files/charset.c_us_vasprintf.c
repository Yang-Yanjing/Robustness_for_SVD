 */
int us_vasprintf( char **ret, const char *format, va_list ap )
{
    locale_t loc = newlocale( LC_NUMERIC_MASK, "C", NULL );
    locale_t oldloc = uselocale( loc );
    int i_rc = vasprintf( ret, format, ap );
    if ( loc != (locale_t)0 )
    {
        uselocale( oldloc );
        freelocale( loc );
    }
    return i_rc;
}
