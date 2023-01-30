/* TODO FIXME nearly the same logic that snapshot code */
char *input_CreateFilename( input_thread_t *input, const char *psz_path, const char *psz_prefix, const char *psz_extension )
{
    char *psz_file;
    DIR *path;
    path = vlc_opendir( psz_path );
    if( path )
    {
        closedir( path );
        char *psz_tmp = str_format( input, psz_prefix );
        if( !psz_tmp )
            return NULL;
        filename_sanitize( psz_tmp );
        if( asprintf( &psz_file, "%s"DIR_SEP"%s%s%s",
                      psz_path, psz_tmp,
                      psz_extension ? "." : "",
                      psz_extension ? psz_extension : "" ) < 0 )
            psz_file = NULL;
        free( psz_tmp );
        return psz_file;
    }
    else
    {
        psz_file = str_format( input, psz_path );
        path_sanitize( psz_file );
        return psz_file;
    }
}
