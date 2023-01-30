/* */
static int FilterMplsLong( const char *psz_name )
{
    return strlen( psz_name ) == strlen( "xxxxx.mpls" ) &&
           !strcmp( &psz_name[5], ".mpls" );
}
