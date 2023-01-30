 */
static int FindArt( playlist_fetcher_t *p_fetcher, input_item_t *p_item )
{
    int i_ret;
    playlist_album_t *p_album = NULL;
    char *psz_artist = input_item_GetArtist( p_item );
    char *psz_album = input_item_GetAlbum( p_item );
    char *psz_title = input_item_GetTitle( p_item );
    if( !psz_title )
        psz_title = input_item_GetName( p_item );
    if( !psz_title && !psz_artist && !psz_album )
        return VLC_EGENERIC;
    free( psz_title );
    /* If we already checked this album in this session, skip */
    if( psz_artist && psz_album )
    {
        FOREACH_ARRAY( playlist_album_t album, p_fetcher->albums )
            if( !strcmp( album.psz_artist, psz_artist ) &&
                !strcmp( album.psz_album, psz_album ) )
            {
                msg_Dbg( p_fetcher->object,
                         " %s - %s has already been searched",
                         psz_artist, psz_album );
                /* TODO-fenrir if we cache art filename too, we can go faster */
                free( psz_artist );
                free( psz_album );
                if( album.b_found )
                {
                    if( !strncmp( album.psz_arturl, "file://", 7 ) )
                        input_item_SetArtURL( p_item, album.psz_arturl );
                    else /* Actually get URL from cache */
                        playlist_FindArtInCache( p_item );
                    return 0;
                }
                else if ( album.e_scope >= p_fetcher->e_scope )
                {
                    return VLC_EGENERIC;
                }
                msg_Dbg( p_fetcher->object,
                         " will search at higher scope, if possible" );
                p_album = &p_fetcher->albums.p_elems[fe_idx];
                psz_artist = psz_album = NULL;
                break;
            }
        FOREACH_END();
    }
    free( psz_artist );
    free( psz_album );
    if ( playlist_FindArtInCacheUsingItemUID( p_item ) != VLC_SUCCESS )
        playlist_FindArtInCache( p_item );
    else
        msg_Dbg( p_fetcher->object, "successfully retrieved arturl by uid" );
    char *psz_arturl = input_item_GetArtURL( p_item );
    if( psz_arturl )
    {
        /* We already have a URL */
        if( !strncmp( psz_arturl, "file://", strlen( "file://" ) ) )
        {
            free( psz_arturl );
            return 0; /* Art is in cache, no need to go further */
        }
        free( psz_arturl );
        /* Art need to be put in cache */
        return 1;
    }
    /* */
    psz_album = input_item_GetAlbum( p_item );
    psz_artist = input_item_GetArtist( p_item );
    if( psz_album && psz_artist )
    {
        msg_Dbg( p_fetcher->object, "searching art for %s - %s",
                 psz_artist, psz_album );
    }
    else
    {
        psz_title = input_item_GetTitle( p_item );
        if( !psz_title )
            psz_title = input_item_GetName( p_item );
        msg_Dbg( p_fetcher->object, "searching art for %s", psz_title );
        free( psz_title );
    }
    /* Fetch the art url */
    i_ret = VLC_EGENERIC;
    vlc_object_t *p_parent = p_fetcher->object;
    meta_fetcher_t *p_finder =
        vlc_custom_create( p_parent, sizeof( *p_finder ), "art finder" );
    if( p_finder != NULL)
    {
        module_t *p_module;
        p_finder->p_item = p_item;
        p_finder->e_scope = p_fetcher->e_scope;
        p_module = module_need( p_finder, "art finder", NULL, false );
        if( p_module )
        {
            module_unneed( p_finder, p_module );
            /* Try immediately if found in cache by download URL */
            if( !playlist_FindArtInCache( p_item ) )
                i_ret = 0;
            else
                i_ret = 1;
        }
        vlc_object_release( p_finder );
    }
    /* Record this album */
    if( psz_artist && psz_album )
    {
        if ( p_album )
        {
            p_album->e_scope = p_fetcher->e_scope;
            free( p_album->psz_arturl );
            p_album->psz_arturl = input_item_GetArtURL( p_item );
            p_album->b_found = (i_ret == VLC_EGENERIC ? false : true );
            free( psz_artist );
            free( psz_album );
        }
        else
        {
            playlist_album_t a;
            a.psz_artist = psz_artist;
            a.psz_album = psz_album;
            a.psz_arturl = input_item_GetArtURL( p_item );
            a.b_found = (i_ret == VLC_EGENERIC ? false : true );
            a.e_scope = p_fetcher->e_scope;
            ARRAY_APPEND( p_fetcher->albums, a );
        }
    }
    else
    {
        free( psz_artist );
        free( psz_album );
    }
    return i_ret;
}
