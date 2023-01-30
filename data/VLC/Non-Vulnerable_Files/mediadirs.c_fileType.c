}
enum type_e fileType( services_discovery_t *p_sd, const char* psz_file )
{
    services_discovery_sys_t *p_sys = p_sd->p_sys;
    enum type_e i_ret = Unknown;
    char* psz_dir = strdup( psz_file );
    char* psz_tmp = strrchr( psz_dir, DIR_SEP_CHAR );
    if( psz_tmp )
        *psz_tmp = '\0';
    int num_dir = sizeof( p_sys->psz_dir ) / sizeof( p_sys->psz_dir[0] );
    for( int i = 0; i < num_dir; i++ )
    {
        char* psz_known_dir = p_sys->psz_dir[i];
        if( psz_known_dir && !strcmp( psz_dir, psz_known_dir ) )
            i_ret = p_sys->i_type;
    }
    free( psz_dir );
    return i_ret;
}
