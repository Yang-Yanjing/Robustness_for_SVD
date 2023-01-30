 *****************************************************************************/
static int FindMeta( vlc_object_t *p_this )
{
    meta_fetcher_t *p_finder = (meta_fetcher_t *)p_this;
    input_item_t *p_item = p_finder->p_item;
    bool b_have_art = false;
    struct stat statinfo;
    char *psz_path = NULL;
    if( !p_item )
        return VLC_EGENERIC;
    char *psz_uri = input_item_GetURI( p_item );
    if( !psz_uri )
        return VLC_EGENERIC;
    if ( *psz_uri && psz_uri[strlen( psz_uri ) - 1] != DIR_SEP_CHAR )
    {
        if ( asprintf( &psz_path, "%s"DIR_SEP, psz_uri ) == -1 )
        {
            free( psz_uri );
            return VLC_EGENERIC;
        }
        char *psz_basedir = make_path( psz_path );
        FREENULL( psz_path );
        if( psz_basedir == NULL )
        {
            free( psz_uri );
            return VLC_EGENERIC;
        }
        if( vlc_stat( psz_basedir, &statinfo ) == 0 && S_ISDIR(statinfo.st_mode) )
            psz_path = psz_basedir;
        else
            free( psz_basedir );
    }
    if ( psz_path == NULL )
    {
        char *psz_basedir = make_path( psz_uri );
        if( psz_basedir == NULL )
        {
            free( psz_uri );
            return VLC_EGENERIC;
        }
        char *psz_buf = strrchr( psz_basedir, DIR_SEP_CHAR );
        if( psz_buf )
            *++psz_buf = '\0';
        else
            *psz_basedir = '\0'; /* relative path */
        psz_path = psz_basedir;
    }
    free( psz_uri );
    for( int i = -1; !b_have_art && i < i_covers; i++ )
    {
        const char *filename;
        char *filebuf, *filepath;
        if( i == -1 ) /* higher priority : configured filename */
        {
            filebuf = var_InheritString( p_this, "album-art-filename" );
            if( filebuf == NULL )
                continue;
            filename = filebuf;
        }
        else
        {
            filename = cover_files[i];
            filebuf = NULL;
        }
        if( asprintf( &filepath, "%s%s", psz_path, filename ) == -1 )
            filepath = NULL;
        free( filebuf );
        if( unlikely(filepath == NULL) )
            continue;
        if( vlc_stat( filepath, &statinfo ) == 0 && S_ISREG(statinfo.st_mode) )
        {
            char *psz_uri = vlc_path2uri( filepath, "file" );
            if( psz_uri )
            {
                input_item_SetArtURL( p_item, psz_uri );
                free( psz_uri );
                b_have_art = true;
            }
        }
        free( filepath );
    }
    free( psz_path );
    return b_have_art ? VLC_SUCCESS : VLC_EGENERIC;
}
