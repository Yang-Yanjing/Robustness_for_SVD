}
static int FilterClpiShort( const char *psz_name )
{
    return strlen( psz_name ) == strlen( "xxxxx.CPI" ) &&
           !strcmp( &psz_name[5], ".CPI" );
}
