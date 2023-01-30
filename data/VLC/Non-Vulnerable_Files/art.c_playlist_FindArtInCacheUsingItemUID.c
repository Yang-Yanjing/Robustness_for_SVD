}
int playlist_FindArtInCacheUsingItemUID( input_item_t *p_item )
{
    char *uid = input_item_GetInfo( p_item, "uid", "md5" );
    if ( ! *uid )
    {
        free( uid );
        return VLC_EGENERIC;
    }
    /* we have an input item uid set */
    bool b_done = false;
    char *psz_byuiddir = GetDirByItemUIDs( uid );
    char *psz_byuidfile = GetFileByItemUID( psz_byuiddir, "arturl" );
    free( psz_byuiddir );
    if( psz_byuidfile )
    {
        FILE *fd = vlc_fopen( psz_byuidfile, "rb" );
        if ( fd )
        {
            char sz_cachefile[2049];
            /* read the cache hash url */
            if ( fgets( sz_cachefile, 2048, fd ) != NULL )
            {
                input_item_SetArtURL( p_item, sz_cachefile );
                b_done = true;
            }
            fclose( fd );
        }
        free( psz_byuidfile );
    }
    free( uid );
    if ( b_done ) return VLC_SUCCESS;
    return VLC_EGENERIC;
}
