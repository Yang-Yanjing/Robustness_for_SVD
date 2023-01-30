}
static int ListSkin_filter( const char * psz_filename )
{
    int i_len = strlen( psz_filename );
    if ( i_len  <= 4 )
        return 0;
    else
        return ! strcmp( psz_filename + i_len - 4, ".vlt" );
}
