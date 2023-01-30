}
static int parser_SetVisibility( char *psz_command, char *psz_end,
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
        int32_t i_vis = 0;
        if( parse_digit( &psz_command, &i_vis ) == VLC_EGENERIC )
            return VLC_EGENERIC;
        p_params->b_visible = (i_vis == 1) ? true : false;
    }
    return VLC_SUCCESS;
}
