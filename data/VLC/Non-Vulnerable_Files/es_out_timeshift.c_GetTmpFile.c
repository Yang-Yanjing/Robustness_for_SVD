}
static FILE *GetTmpFile( char **ppsz_file, const char *psz_path )
{
    char *psz_name;
    int fd;
    FILE *f;
    /* */
    *ppsz_file = NULL;
    if( asprintf( &psz_name, "%s"DIR_SEP"vlc-timeshift.XXXXXX", psz_path ) < 0 )
        return NULL;
    /* */
    fd = vlc_mkstemp( psz_name );
    *ppsz_file = psz_name;
    if( fd < 0 )
        return NULL;
    /* */
    f = fdopen( fd, "w+b" );
    if( !f )
        close( fd );
    return f;
}
