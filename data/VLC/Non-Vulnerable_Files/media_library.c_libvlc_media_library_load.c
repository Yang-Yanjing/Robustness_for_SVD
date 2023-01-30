 **************************************************************************/
int libvlc_media_library_load( libvlc_media_library_t * p_mlib )
{
    char *psz_datadir = config_GetUserDir( VLC_DATA_DIR );
    char * psz_uri;
    if( psz_datadir == NULL
     || asprintf( &psz_uri, "file/xspf-open://%s" DIR_SEP "ml.xsp",
                  psz_datadir ) == -1 )
        psz_uri = NULL;
    free( psz_datadir );
    if( psz_uri == NULL )
    {
        libvlc_printerr( "Not enough memory" );
        return -1;
    }
    if( p_mlib->p_mlist )
        libvlc_media_list_release( p_mlib->p_mlist );
    p_mlib->p_mlist = libvlc_media_list_new( p_mlib->p_libvlc_instance );
    int ret = libvlc_media_list_add_file_content( p_mlib->p_mlist, psz_uri );
    free( psz_uri );
    return ret;
}
