 *****************************************************************************/
playlist_fetcher_t *playlist_fetcher_New( vlc_object_t *parent )
{
    playlist_fetcher_t *p_fetcher = malloc( sizeof(*p_fetcher) );
    if( !p_fetcher )
        return NULL;
    p_fetcher->object = parent;
    vlc_mutex_init( &p_fetcher->lock );
    vlc_cond_init( &p_fetcher->wait );
    p_fetcher->b_live = false;
    bool b_access = var_InheritBool( parent, "metadata-network-access" );
    if ( !b_access )
        b_access = ( var_InheritInteger( parent, "album-art" ) == ALBUM_ART_ALL );
    p_fetcher->e_scope = ( b_access ) ? FETCHER_SCOPE_ANY : FETCHER_SCOPE_LOCAL;
    memset( p_fetcher->p_waiting_head, 0, PASS_COUNT * sizeof(fetcher_entry_t *) );
    memset( p_fetcher->p_waiting_tail, 0, PASS_COUNT * sizeof(fetcher_entry_t *) );
    ARRAY_INIT( p_fetcher->albums );
    return p_fetcher;
}
