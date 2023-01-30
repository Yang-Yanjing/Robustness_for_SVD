}
static char *ArtCachePath( input_item_t *p_item )
{
    char* psz_path = NULL;
    const char *psz_artist;
    const char *psz_album;
    const char *psz_arturl;
    const char *psz_title;
    vlc_mutex_lock( &p_item->lock );
    if( !p_item->p_meta )
        p_item->p_meta = vlc_meta_New();
    if( !p_item->p_meta )
        goto end;
    psz_artist = vlc_meta_Get( p_item->p_meta, vlc_meta_Artist );
    psz_album = vlc_meta_Get( p_item->p_meta, vlc_meta_Album );
    psz_arturl = vlc_meta_Get( p_item->p_meta, vlc_meta_ArtworkURL );
    psz_title = vlc_meta_Get( p_item->p_meta, vlc_meta_Title );
    if( !psz_title )
        psz_title = p_item->psz_name;
    if( (EMPTY_STR(psz_artist) || EMPTY_STR(psz_album) ) && !psz_arturl )
        goto end;
    psz_path = ArtCacheGetDirPath( psz_arturl, psz_artist, psz_album, psz_title );
end:
    vlc_mutex_unlock( &p_item->lock );
    return psz_path;
}
