/* */
static int FilterClpiLong( const char *psz_name )
{
    return strlen( psz_name ) == strlen( "xxxxx.clpi" ) &&
           !strcmp( &psz_name[5], ".clpi" );
}
