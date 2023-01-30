 */
int us_asprintf( char **ret, const char *format, ... )
{
    va_list ap;
    int i_rc;
    va_start( ap, format );
    i_rc = us_vasprintf( ret, format, ap );
    va_end( ap );
    return i_rc;
}
