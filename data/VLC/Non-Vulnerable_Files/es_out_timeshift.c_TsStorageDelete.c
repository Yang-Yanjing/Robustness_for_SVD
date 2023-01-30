}
static void TsStorageDelete( ts_storage_t *p_storage )
{
    while( p_storage->i_cmd_r < p_storage->i_cmd_w )
    {
        ts_cmd_t cmd;
        TsStoragePopCmd( p_storage, &cmd, true );
        CmdClean( &cmd );
    }
    free( p_storage->p_cmd );
    if( p_storage->p_filer )
        fclose( p_storage->p_filer );
    if( p_storage->p_filew )
        fclose( p_storage->p_filew );
    if( p_storage->psz_file )
    {
        vlc_unlink( p_storage->psz_file );
        free( p_storage->psz_file );
    }
    free( p_storage );
}
