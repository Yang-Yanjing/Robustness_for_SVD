/* Removes an extra interface from the configuration */
void config_RemoveIntf( vlc_object_t *p_this, const char *psz_intf )
{
    assert( psz_intf );
    char *psz_config, *psz_parser;
    size_t i_len = strlen( psz_intf );
    psz_config = psz_parser = config_GetPsz( p_this->p_libvlc, "extraintf" );
    while( psz_parser )
    {
        if( !strncmp( psz_intf, psz_parser, i_len ) )
        {
            char *psz_newconfig;
            char *psz_end = psz_parser + i_len;
            if( *psz_end == ':' ) psz_end++;
            *psz_parser = '\0';
            if( asprintf( &psz_newconfig, "%s%s", psz_config, psz_end ) != -1 )
            {
                config_PutPsz( p_this->p_libvlc, "extraintf", psz_newconfig );
                free( psz_newconfig );
            }
            break;
        }
        psz_parser = strchr( psz_parser, ':' );
        if( psz_parser ) psz_parser++; /* skip the ':' */
    }
    free( psz_config );
    psz_config = psz_parser = config_GetPsz( p_this->p_libvlc, "control" );
    while( psz_parser )
    {
        if( !strncmp( psz_intf, psz_parser, i_len ) )
        {
            char *psz_newconfig;
            char *psz_end = psz_parser + i_len;
            if( *psz_end == ':' ) psz_end++;
            *psz_parser = '\0';
            if( asprintf( &psz_newconfig, "%s%s", psz_config, psz_end ) != -1 )
            {
                config_PutPsz( p_this->p_libvlc, "control", psz_newconfig );
                free( psz_newconfig );
            }
            break;
        }
        psz_parser = strchr( psz_parser, ':' );
        if( psz_parser ) psz_parser++; /* skip the ':' */
    }
    free( psz_config );
}
