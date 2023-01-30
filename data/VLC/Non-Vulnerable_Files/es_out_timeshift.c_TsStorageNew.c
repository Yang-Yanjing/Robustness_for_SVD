 *****************************************************************************/
static ts_storage_t *TsStorageNew( const char *psz_tmp_path, int64_t i_tmp_size_max )
{
    ts_storage_t *p_storage = calloc( 1, sizeof(ts_storage_t) );
    if( !p_storage )
        return NULL;
    /* */
    p_storage->p_next = NULL;
    /* */
    p_storage->i_file_max = i_tmp_size_max;
    p_storage->i_file_size = 0;
    p_storage->p_filew = GetTmpFile( &p_storage->psz_file, psz_tmp_path );
    if( p_storage->psz_file )
        p_storage->p_filer = vlc_fopen( p_storage->psz_file, "rb" );
    /* */
    p_storage->i_cmd_w = 0;
    p_storage->i_cmd_r = 0;
    p_storage->i_cmd_max = 30000;
    p_storage->p_cmd = malloc( p_storage->i_cmd_max * sizeof(*p_storage->p_cmd) );
    //fprintf( stderr, "\nSTORAGE name=%s size=%d KiB\n", p_storage->psz_file, p_storage->i_cmd_max * sizeof(*p_storage->p_cmd) /1024 );
    if( !p_storage->p_cmd || !p_storage->p_filew || !p_storage->p_filer )
    {
        TsStorageDelete( p_storage );
        return NULL;
    }
    return p_storage;
}
