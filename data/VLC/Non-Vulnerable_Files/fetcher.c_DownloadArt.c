 */
static int DownloadArt( playlist_fetcher_t *p_fetcher, input_item_t *p_item )
{
    char *psz_arturl = input_item_GetArtURL( p_item );
    assert( *psz_arturl );
    if( !strncasecmp( psz_arturl , "file://", 7 ) )
    {
        msg_Dbg( p_fetcher->object,
                 "Album art is local file, no need to cache" );
        free( psz_arturl );
        return VLC_SUCCESS;
    }
    if( !strncmp( psz_arturl , "APIC", 4 ) )
    {
        msg_Warn( p_fetcher->object, "APIC fetch not supported yet" );
        goto error;
    }
    stream_t *p_stream = stream_UrlNew( p_fetcher->object, psz_arturl );
    if( !p_stream )
        goto error;
    uint8_t *p_data = NULL;
    int i_data = 0;
    for( ;; )
    {
        int i_read = 65536;
        if( i_data >= INT_MAX - i_read )
            break;
        p_data = realloc_or_free( p_data, i_data + i_read );
        if( !p_data )
            break;
        i_read = stream_Read( p_stream, &p_data[i_data], i_read );
        if( i_read <= 0 )
            break;
        i_data += i_read;
    }
    stream_Delete( p_stream );
    if( p_data && i_data > 0 )
    {
        char *psz_type = strrchr( psz_arturl, '.' );
        if( psz_type && strlen( psz_type ) > 5 )
            psz_type = NULL; /* remove extension if it's > to 4 characters */
        playlist_SaveArt( p_fetcher->object, p_item,
                          p_data, i_data, psz_type );
    }
    free( p_data );
    free( psz_arturl );
    return VLC_SUCCESS;
error:
    free( psz_arturl );
    return VLC_EGENERIC;
}
