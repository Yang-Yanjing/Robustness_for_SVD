}
static char * GetFileByItemUID( char *psz_dir, const char *psz_type )
{
    char *psz_file;
    if( asprintf( &psz_file, "%s" DIR_SEP "%s", psz_dir, psz_type ) == -1 )
    {
        psz_file = NULL;
    }
    return psz_file;
}
