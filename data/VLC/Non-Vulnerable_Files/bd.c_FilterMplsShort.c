}
static int FilterMplsShort( const char *psz_name )
{
    return strlen( psz_name ) == strlen( "xxxxx.MPL" ) &&
           !strcmp( &psz_name[5], ".MPL" );
}
