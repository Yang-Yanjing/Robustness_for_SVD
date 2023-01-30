 *****************************************************************************/
static void InputGetExtraFilesPattern( input_thread_t *p_input,
                                       int *pi_list, char ***pppsz_list,
                                       const char *psz_path,
                                       const char *psz_match,
                                       const char *psz_format,
                                       int i_start, int i_stop )
{
    int i_list;
    char **ppsz_list;
    TAB_INIT( i_list, ppsz_list );
    char *psz_base = strdup( psz_path );
    if( !psz_base )
        goto exit;
    /* Remove the extension */
    char *psz_end = &psz_base[strlen(psz_base)-strlen(psz_match)];
    assert( psz_end >= psz_base);
    *psz_end = '\0';
    /* Try to list files */
    for( int i = i_start; i <= i_stop; i++ )
    {
        struct stat st;
        char *psz_file;
        if( asprintf( &psz_file, psz_format, psz_base, i ) < 0 )
            break;
        char *psz_tmp_path = get_path( psz_file );
        if( vlc_stat( psz_tmp_path, &st ) || !S_ISREG( st.st_mode ) || !st.st_size )
        {
            free( psz_file );
            free( psz_tmp_path );
            break;
        }
        msg_Dbg( p_input, "Detected extra file `%s'", psz_file );
        TAB_APPEND( i_list, ppsz_list, psz_file );
        free( psz_tmp_path );
    }
    free( psz_base );
exit:
    *pi_list = i_list;
    *pppsz_list = ppsz_list;
}
