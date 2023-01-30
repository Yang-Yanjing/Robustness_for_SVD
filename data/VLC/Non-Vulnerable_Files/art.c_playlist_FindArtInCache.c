/* */
int playlist_FindArtInCache( input_item_t *p_item )
{
    char *psz_path = ArtCachePath( p_item );
    if( !psz_path )
        return VLC_EGENERIC;
    /* Check if file exists */
    DIR *p_dir = vlc_opendir( psz_path );
    if( !p_dir )
    {
        free( psz_path );
        return VLC_EGENERIC;
    }
    bool b_found = false;
    const char *psz_filename;
    while( !b_found && (psz_filename = vlc_readdir( p_dir )) )
    {
        if( !strncmp( psz_filename, "art", 3 ) )
        {
            char *psz_file;
            if( asprintf( &psz_file, "%s" DIR_SEP "%s",
                          psz_path, psz_filename ) != -1 )
            {
                char *psz_uri = vlc_path2uri( psz_file, "file" );
                if( psz_uri )
                {
                    input_item_SetArtURL( p_item, psz_uri );
                    free( psz_uri );
                }
                free( psz_file );
            }
            b_found = true;
        }
    }
    /* */
    closedir( p_dir );
    free( psz_path );
    return b_found ? VLC_SUCCESS : VLC_EGENERIC;
}
