 *****************************************************************************/
static int skip_space( char **psz_command )
{
    char *psz_temp = *psz_command;
    while( isspace( (unsigned char)*psz_temp ) )
    {
        ++psz_temp;
    }
    if( psz_temp == *psz_command )
    {
        return VLC_EGENERIC;
    }
    *psz_command = psz_temp;
    return VLC_SUCCESS;
}
