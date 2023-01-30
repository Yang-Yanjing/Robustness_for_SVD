}
static void RemoveBase64Padding( char *str )
{
    char *ps_pos = strchr( str, '=' );
    if ( ps_pos != NULL )
        *ps_pos = '\0';
}
