}
static char *MarqueeReadFile( filter_t *obj, const char *path )
{
    FILE *stream = vlc_fopen( path, "rt" );
    if( stream == NULL )
    {
        msg_Err( obj, "cannot open %s: %s", path, vlc_strerror_c(errno) );
        return NULL;
    }
    char *line = NULL;
    ssize_t len = getline( &line, &(size_t){ 0 }, stream );
    if( len == -1 )
    {
        msg_Err( obj, "cannot read %s: %s", path, vlc_strerror_c(errno) );
        clearerr( stream );
        line = NULL;
    }
    fclose( stream );
    if( len >= 1 && line[len - 1] == '\n' )
        line[--len]  = '\0';
    return line;
}
