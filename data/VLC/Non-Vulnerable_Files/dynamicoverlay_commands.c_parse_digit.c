}
static int parse_digit( char **psz_command, int32_t *value )
{
    char *psz_temp;
    *value = strtol( *psz_command, &psz_temp, 10 );
    if( psz_temp == *psz_command )
    {
        return VLC_EGENERIC;
    }
    *psz_command = psz_temp;
    return VLC_SUCCESS;
}
