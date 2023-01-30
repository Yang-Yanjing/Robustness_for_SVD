}
static char *ArtCacheName( input_item_t *p_item, const char *psz_type )
{
    char *psz_path = ArtCachePath( p_item );
    if( !psz_path )
        return NULL;
    ArtCacheCreateDir( psz_path );
    char *psz_ext = strdup( psz_type ? psz_type : "" );
    filename_sanitize( psz_ext );
    char *psz_filename;
    if( asprintf( &psz_filename, "%s" DIR_SEP "art%s", psz_path, psz_ext ) < 0 )
        psz_filename = NULL;
    free( psz_ext );
    free( psz_path );
    return psz_filename;
}
