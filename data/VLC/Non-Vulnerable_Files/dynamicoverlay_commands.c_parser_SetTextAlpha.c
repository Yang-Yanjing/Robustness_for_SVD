}
static int parser_SetTextAlpha( char *psz_command, char *psz_end,
                                commandparams_t *p_params )
{
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
        if( parse_digit( &psz_command, &p_params->fontstyle.i_font_alpha ) == VLC_EGENERIC )
            return VLC_EGENERIC;
    }
    return VLC_SUCCESS;
}
