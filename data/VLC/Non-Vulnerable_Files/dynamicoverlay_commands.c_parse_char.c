}
static int parse_char( char **psz_command, char **psz_end,
                       int count, char *psz_value )
{
    if( *psz_end - *psz_command < count )
    {
        return VLC_EGENERIC;
    }
    memcpy( psz_value, *psz_command, count );
    *psz_command += count;
    return VLC_SUCCESS;
}
