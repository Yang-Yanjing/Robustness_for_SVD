}
void playlist_fetcher_Delete( playlist_fetcher_t *p_fetcher )
{
    fetcher_entry_t *p_next;
    vlc_mutex_lock( &p_fetcher->lock );
    /* Remove any left-over item, the fetcher will exit */
    for ( int i_queue=0; i_queue<PASS_COUNT; i_queue++ )
    {
        while( p_fetcher->p_waiting_head[i_queue] )
        {
            p_next = p_fetcher->p_waiting_head[i_queue]->p_next;
            vlc_gc_decref( p_fetcher->p_waiting_head[i_queue]->p_item );
            free( p_fetcher->p_waiting_head[i_queue] );
            p_fetcher->p_waiting_head[i_queue] = p_next;
        }
        p_fetcher->p_waiting_head[i_queue] = NULL;
    }
    while( p_fetcher->b_live )
        vlc_cond_wait( &p_fetcher->wait, &p_fetcher->lock );
    vlc_mutex_unlock( &p_fetcher->lock );
    vlc_cond_destroy( &p_fetcher->wait );
    vlc_mutex_destroy( &p_fetcher->lock );
    playlist_album_t album;
    FOREACH_ARRAY( album, p_fetcher->albums )
        free( album.psz_album );
        free( album.psz_artist );
        free( album.psz_arturl );
    FOREACH_END()
    ARRAY_RESET( p_fetcher->albums );
    free( p_fetcher );
}
