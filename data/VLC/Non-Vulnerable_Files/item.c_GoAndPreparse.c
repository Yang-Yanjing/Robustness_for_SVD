/* Enqueue an item for preparsing, and play it, if needed */
static void GoAndPreparse( playlist_t *p_playlist, int i_mode,
                           playlist_item_t *p_item )
{
    playlist_private_t *sys = pl_priv(p_playlist);
    PL_ASSERT_LOCKED;
    if( (i_mode & PLAYLIST_GO ) )
    {
        sys->request.b_request = true;
        sys->request.i_skip = 0;
        sys->request.p_item = p_item;
        if( sys->p_input != NULL )
            input_Stop( sys->p_input, true );
        sys->request.i_status = PLAYLIST_RUNNING;
        vlc_cond_signal( &sys->signal );
    }
    /* Preparse if no artist/album info, and hasn't been preparsed allready
       and if user has some preparsing option (auto-preparse variable)
       enabled*/
    char *psz_artist = input_item_GetArtist( p_item->p_input );
    char *psz_album = input_item_GetAlbum( p_item->p_input );
    if( sys->p_preparser != NULL && !input_item_IsPreparsed( p_item->p_input )
     && (EMPTY_STR(psz_artist) || EMPTY_STR(psz_album)) )
        playlist_preparser_Push( sys->p_preparser, p_item->p_input, 0 );
    free( psz_artist );
    free( psz_album );
}
