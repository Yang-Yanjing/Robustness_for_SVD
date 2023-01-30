}
static int parser_SetTextColor( char *psz_command, char *psz_end,
                                commandparams_t *p_params )
{
    int r = 0, g = 0, b = 0;
    VLC_UNUSED(psz_end);
    skip_space( &psz_command );
    if( isdigit( (unsigned char)*psz_command ) )
    {
        if( parse_digit( &psz_command, &p_params->i_id ) == VLC_EGENERIC )
            return VLC_EGENERIC;
    }
    skip_space( &psz_command );
    if( isdigit( (unsigned char)*psz_command ) )
    {
        if( parse_digit( &psz_command, &r ) == VLC_EGENERIC )
            return VLC_EGENERIC;
    }
    skip_space( &psz_command );
    if( isdigit( (unsigned char)*psz_command ) )
    {
        if( parse_digit( &psz_command, &g ) == VLC_EGENERIC )
            return VLC_EGENERIC;
    }
    skip_space( &psz_command );
    if( isdigit( (unsigned char)*psz_command ) )
    {
        if( parse_digit( &psz_command, &b ) == VLC_EGENERIC )
            return VLC_EGENERIC;
    }
    p_params->fontstyle.i_font_color = (r<<16) | (g<<8) | (b<<0);
    return VLC_SUCCESS;
}
