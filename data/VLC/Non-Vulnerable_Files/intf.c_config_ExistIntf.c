 */
bool config_ExistIntf( vlc_object_t *p_this, const char *psz_intf )
{
    assert( psz_intf );
    char *psz_config, *psz_parser;
    size_t i_len = strlen( psz_intf );
    psz_config = psz_parser = config_GetPsz( p_this->p_libvlc, "extraintf" );
    while( psz_parser )
    {
        if( !strncmp( psz_parser, psz_intf, i_len ) )
        {
            free( psz_config );
            return true;
        }
        psz_parser = strchr( psz_parser, ':' );
        if( psz_parser ) psz_parser++; /* skip the ':' */
    }
    free( psz_config );
    psz_config = psz_parser = config_GetPsz( p_this->p_libvlc, "control" );
    while( psz_parser )
    {
        if( !strncmp( psz_parser, psz_intf, i_len ) )
        {
            free( psz_config );
            return true;
        }
        psz_parser = strchr( psz_parser, ':' );
        if( psz_parser ) psz_parser++; /* skip the ':' */
    }
    free( psz_config );
    return false;
}
