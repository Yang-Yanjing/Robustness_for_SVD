}
static void PlayBookmark( intf_thread_t *p_intf, int i_num )
{
    char *psz_bookmark_name;
    if( asprintf( &psz_bookmark_name, "bookmark%i", i_num ) == -1 )
        return;
    playlist_t *p_playlist = pl_Get( p_intf );
    char *psz_bookmark = var_CreateGetString( p_intf, psz_bookmark_name );
    PL_LOCK;
    FOREACH_ARRAY( playlist_item_t *p_item, p_playlist->items )
        char *psz_uri = input_item_GetURI( p_item->p_input );
        if( !strcmp( psz_bookmark, psz_uri ) )
        {
            free( psz_uri );
            playlist_Control( p_playlist, PLAYLIST_VIEWPLAY, pl_Locked,
                              NULL, p_item );
            break;
        }
        else
            free( psz_uri );
    FOREACH_END();
    PL_UNLOCK;
    free( psz_bookmark );
    free( psz_bookmark_name );
}
