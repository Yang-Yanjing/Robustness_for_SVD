}
static int parser_DataSharedMem( char *psz_command,
                                 char *psz_end,
                                 commandparams_t *p_params )
{
    /* Parse: 0 128 128 RGBA 9404459 */
    skip_space( &psz_command );
    if( isdigit( (unsigned char)*psz_command ) )
    {
        if( parse_digit( &psz_command, &p_params->i_id ) == VLC_EGENERIC )
            return VLC_EGENERIC;
    }
    skip_space( &psz_command );
    if( isdigit( (unsigned char)*psz_command ) )
    {
        if( parse_digit( &psz_command, &p_params->i_width ) == VLC_EGENERIC )
            return VLC_EGENERIC;
    }
    skip_space( &psz_command );
    if( isdigit( (unsigned char)*psz_command ) )
    {
        if( parse_digit( &psz_command, &p_params->i_height ) == VLC_EGENERIC )
            return VLC_EGENERIC;
    }
    skip_space( &psz_command );
    if( isascii( (unsigned char)*psz_command ) )
    {
        if( parse_char( &psz_command, &psz_end, 4, (char*)&p_params->fourcc )
            == VLC_EGENERIC )
            return VLC_EGENERIC;
    }
    skip_space( &psz_command );
    if( isdigit( (unsigned char)*psz_command ) )
    {
        if( parse_digit( &psz_command, &p_params->i_shmid ) == VLC_EGENERIC )
            return VLC_EGENERIC;
    }
    return VLC_SUCCESS;
}
