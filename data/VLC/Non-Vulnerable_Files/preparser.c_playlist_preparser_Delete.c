}
void playlist_preparser_Delete( playlist_preparser_t *p_preparser )
{
    vlc_mutex_lock( &p_preparser->lock );
    /* Remove pending item to speed up preparser thread exit */
    while( p_preparser->i_waiting > 0 )
    {
        vlc_gc_decref( p_preparser->pp_waiting[0] );
        REMOVE_ELEM( p_preparser->pp_waiting, p_preparser->i_waiting, 0 );
    }
    while( p_preparser->b_live )
        vlc_cond_wait( &p_preparser->wait, &p_preparser->lock );
    vlc_mutex_unlock( &p_preparser->lock );
    /* Destroy the item preparser */
    vlc_cond_destroy( &p_preparser->wait );
    vlc_mutex_destroy( &p_preparser->lock );
    if( p_preparser->p_fetcher != NULL )
        playlist_fetcher_Delete( p_preparser->p_fetcher );
    free( p_preparser );
}
