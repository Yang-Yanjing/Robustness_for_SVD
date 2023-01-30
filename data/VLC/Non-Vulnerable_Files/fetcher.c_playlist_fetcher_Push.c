}
void playlist_fetcher_Push( playlist_fetcher_t *p_fetcher, input_item_t *p_item,
                            input_item_meta_request_option_t i_options )
{
    fetcher_entry_t *p_entry = malloc( sizeof(fetcher_entry_t) );
    if ( !p_entry ) return;
    vlc_gc_incref( p_item );
    p_entry->p_item = p_item;
    p_entry->p_next = NULL;
    p_entry->i_options = i_options;
    vlc_mutex_lock( &p_fetcher->lock );
    /* Append last */
    if ( p_fetcher->p_waiting_head[PASS1_LOCAL] )
        p_fetcher->p_waiting_tail[PASS1_LOCAL]->p_next = p_entry;
    else
        p_fetcher->p_waiting_head[PASS1_LOCAL] = p_entry;
    p_fetcher->p_waiting_tail[PASS1_LOCAL] = p_entry;
    if( !p_fetcher->b_live )
    {
        assert( p_fetcher->p_waiting_head[PASS1_LOCAL] );
        if( vlc_clone_detach( NULL, Thread, p_fetcher,
                              VLC_THREAD_PRIORITY_LOW ) )
            msg_Err( p_fetcher->object,
                     "cannot spawn secondary preparse thread" );
        else
            p_fetcher->b_live = true;
    }
    vlc_mutex_unlock( &p_fetcher->lock );
}
