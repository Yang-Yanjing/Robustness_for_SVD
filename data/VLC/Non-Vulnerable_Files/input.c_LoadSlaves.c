}
static void LoadSlaves( input_thread_t *p_input )
{
    char *psz = var_GetNonEmptyString( p_input, "input-slave" );
    if( !psz )
        return;
    char *psz_org = psz;
    while( psz && *psz )
    {
        while( *psz == ' ' || *psz == '#' )
            psz++;
        char *psz_delim = strchr( psz, '#' );
        if( psz_delim )
            *psz_delim++ = '\0';
        if( *psz == 0 )
            break;
        char *uri = strstr(psz, "://")
                                   ? strdup( psz ) : vlc_path2uri( psz, NULL );
        psz = psz_delim;
        if( uri == NULL )
            continue;
        msg_Dbg( p_input, "adding slave input '%s'", uri );
        input_source_t *p_slave = InputSourceNew( p_input );
        if( p_slave && !InputSourceInit( p_input, p_slave, uri, NULL, false ) )
            TAB_APPEND( p_input->p->i_slave, p_input->p->slave, p_slave );
        else
            free( p_slave );
        free( uri );
    }
    free( psz_org );
}
