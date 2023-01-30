}
static void *Thread( void *p_data )
{
    playlist_fetcher_t *p_fetcher = p_data;
    vlc_object_t *obj = p_fetcher->object;
    fetcher_pass_t e_pass = PASS1_LOCAL;
    for( ;; )
    {
        fetcher_entry_t *p_entry = NULL;
        vlc_mutex_lock( &p_fetcher->lock );
        for ( int i=0; i<PASS_COUNT; i++ )
        {
            if ( p_fetcher->p_waiting_head[i] )
            {
                e_pass = i;
                break;
            }
        }
        if( p_fetcher->p_waiting_head[e_pass] )
        {
            p_entry = p_fetcher->p_waiting_head[e_pass];
            p_fetcher->p_waiting_head[e_pass] = p_entry->p_next;
            if ( p_entry->p_next == NULL )
                p_fetcher->p_waiting_tail[e_pass] = NULL;
            p_entry->p_next = NULL;
        }
        else
        {
            p_fetcher->b_live = false;
            vlc_cond_signal( &p_fetcher->wait );
        }
        vlc_mutex_unlock( &p_fetcher->lock );
        if( !p_entry )
            break;
        meta_fetcher_scope_t e_prev_scope = p_fetcher->e_scope;
        /* scope override */
        switch ( p_entry->i_options ) {
        case META_REQUEST_OPTION_SCOPE_ANY:
            p_fetcher->e_scope = FETCHER_SCOPE_ANY;
            break;
        case META_REQUEST_OPTION_SCOPE_LOCAL:
            p_fetcher->e_scope = FETCHER_SCOPE_LOCAL;
            break;
        case META_REQUEST_OPTION_SCOPE_NETWORK:
            p_fetcher->e_scope = FETCHER_SCOPE_NETWORK;
            break;
        case META_REQUEST_OPTION_NONE:
        default:
            break;
        }
        /* Triggers "meta fetcher", eventually fetch meta on the network.
         * They are identical to "meta reader" expect that may actually
         * takes time. That's why they are running here.
         * The result of this fetch is not cached. */
        int i_ret = -1;
        if( e_pass == PASS1_LOCAL && ( p_fetcher->e_scope & FETCHER_SCOPE_LOCAL ) )
        {
            /* only fetch from local */
            p_fetcher->e_scope = FETCHER_SCOPE_LOCAL;
        }
        else if( e_pass == PASS2_NETWORK && ( p_fetcher->e_scope & FETCHER_SCOPE_NETWORK ) )
        {
            /* only fetch from network */
            p_fetcher->e_scope = FETCHER_SCOPE_NETWORK;
        }
        else
            p_fetcher->e_scope = 0;
        if ( p_fetcher->e_scope & FETCHER_SCOPE_ANY )
        {
            FetchMeta( p_fetcher, p_entry->p_item );
            i_ret = FindArt( p_fetcher, p_entry->p_item );
            switch( i_ret )
            {
            case 1: /* Found, need to dl */
                i_ret = DownloadArt( p_fetcher, p_entry->p_item );
                break;
            case 0: /* Is in cache */
                i_ret = VLC_SUCCESS;
                //ft
            default:// error
                break;
            }
        }
        p_fetcher->e_scope = e_prev_scope;
        /* */
        if ( i_ret != VLC_SUCCESS && (e_pass != PASS2_NETWORK) )
        {
            /* Move our entry to next pass queue */
            vlc_mutex_lock( &p_fetcher->lock );
            if ( p_fetcher->p_waiting_head[e_pass + 1] )
                p_fetcher->p_waiting_tail[e_pass + 1]->p_next = p_entry;
            else
                p_fetcher->p_waiting_head[e_pass + 1] = p_entry;
            p_fetcher->p_waiting_tail[e_pass + 1] = p_entry;
            vlc_mutex_unlock( &p_fetcher->lock );
        }
        else
        {
            /* */
            char *psz_name = input_item_GetName( p_entry->p_item );
            if( i_ret == VLC_SUCCESS ) /* Art is now in cache */
            {
                msg_Dbg( obj, "found art for %s in cache", psz_name );
                input_item_SetArtFetched( p_entry->p_item, true );
                var_SetAddress( obj, "item-change", p_entry->p_item );
            }
            else
            {
                msg_Dbg( obj, "art not found for %s", psz_name );
                input_item_SetArtNotFound( p_entry->p_item, true );
            }
            free( psz_name );
            vlc_gc_decref( p_entry->p_item );
            free( p_entry );
        }
    }
    return NULL;
}
