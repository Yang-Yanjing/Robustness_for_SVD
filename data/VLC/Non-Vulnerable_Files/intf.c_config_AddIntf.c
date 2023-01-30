/* Adds an extra interface to the configuration */
void config_AddIntf( vlc_object_t *p_this, const char *psz_intf )
{
    assert( psz_intf );
    char *psz_config, *psz_parser;
    size_t i_len = strlen( psz_intf );
    psz_config = psz_parser = config_GetPsz( p_this->p_libvlc, "control" );
    while( psz_parser )
    {
        if( !strncmp( psz_intf, psz_parser, i_len ) )
        {
            free( psz_config );
            return;
        }
        psz_parser = strchr( psz_parser, ':' );
        if( psz_parser ) psz_parser++; /* skip the ':' */
    }
    free( psz_config );
    psz_config = psz_parser = config_GetPsz( p_this->p_libvlc, "extraintf" );
    while( psz_parser )
    {
        if( !strncmp( psz_intf, psz_parser, i_len ) )
        {
            free( psz_config );
            return;
        }
        psz_parser = strchr( psz_parser, ':' );
        if( psz_parser ) psz_parser++; /* skip the ':' */
    }
    /* interface not found in the config, let's add it */
    if( psz_config && strlen( psz_config ) > 0 )
    {
        char *psz_newconfig;
        if( asprintf( &psz_newconfig, "%s:%s", psz_config, psz_intf ) != -1 )
        {
            config_PutPsz( p_this->p_libvlc, "extraintf", psz_newconfig );
            free( psz_newconfig );
        }
    }
    else
        config_PutPsz( p_this->p_libvlc, "extraintf", psz_intf );
    free( psz_config );
}
