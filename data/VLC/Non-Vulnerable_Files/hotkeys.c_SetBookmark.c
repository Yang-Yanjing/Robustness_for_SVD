}
static void SetBookmark( intf_thread_t *p_intf, int i_num )
{
    char *psz_bookmark_name;
    char *psz_uri = NULL;
    if( asprintf( &psz_bookmark_name, "bookmark%i", i_num ) == -1 )
        return;
    playlist_t *p_playlist = pl_Get( p_intf );
    var_Create( p_intf, psz_bookmark_name,
                VLC_VAR_STRING|VLC_VAR_DOINHERIT );
    PL_LOCK;
    playlist_item_t * p_item = playlist_CurrentPlayingItem( p_playlist );
    if( p_item ) psz_uri = input_item_GetURI( p_item->p_input );
    PL_UNLOCK;
    if( p_item )
    {
        config_PutPsz( p_intf, psz_bookmark_name, psz_uri);
        msg_Info( p_intf, "setting playlist bookmark %i to %s", i_num, psz_uri);
    }
    free( psz_uri );
    free( psz_bookmark_name );
}
