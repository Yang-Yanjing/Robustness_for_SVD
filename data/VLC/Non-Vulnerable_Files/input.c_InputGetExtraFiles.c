}
static void InputGetExtraFiles( input_thread_t *p_input,
                                int *pi_list, char ***pppsz_list,
                                const char *psz_access, const char *psz_path )
{
    static const struct
    {
        const char *psz_match;
        const char *psz_format;
        int i_start;
        int i_stop;
    } p_pattern[] = {
        /* XXX the order is important */
        { ".001",         "%s.%.3d",        2, 999 },
        { NULL, NULL, 0, 0 }
    };
    TAB_INIT( *pi_list, *pppsz_list );
    if( ( psz_access && *psz_access && strcmp( psz_access, "file" ) ) || !psz_path )
        return;
    const size_t i_path = strlen(psz_path);
    for( int i = 0; p_pattern[i].psz_match != NULL; i++ )
    {
        const size_t i_ext = strlen(p_pattern[i].psz_match );
        if( i_path < i_ext )
            continue;
        if( !strcmp( &psz_path[i_path-i_ext], p_pattern[i].psz_match ) )
        {
            InputGetExtraFilesPattern( p_input, pi_list, pppsz_list,
                                       psz_path,
                                       p_pattern[i].psz_match, p_pattern[i].psz_format,
                                       p_pattern[i].i_start, p_pattern[i].i_stop );
            return;
        }
    }
}
