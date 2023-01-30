}
void* sms_Thread( void *p_this )
{
    stream_t *s = (stream_t *)p_this;
    stream_sys_t *p_sys = s->p_sys;
    sms_stream_t *sms = NULL;
    chunk_t *chunk;
    int canc = vlc_savecancel();
    /* We compute the average bandwidth of the 4 last downloaded
     * chunks, but feel free to replace '4' by whatever you wish */
    p_sys->bws = sms_queue_init( 4 );
    if( !p_sys->bws )
        goto cancel;
    chunk_t *init_ck = build_init_chunk( s );
    if( !init_ck )
        goto cancel;
    vlc_mutex_lock( &p_sys->download.lock_wait );
    vlc_array_append( p_sys->download.chunks, init_ck );
    vlc_array_append( p_sys->init_chunks, init_ck );
    vlc_mutex_unlock( &p_sys->download.lock_wait );
    p_sys->download.next_chunk_offset = init_ck->size;
    /* XXX Sometimes, the video stream is cut into pieces of one exact length,
     * while the audio stream fragments can't be made to match exactly,
     * and for some reason the n^th advertised video fragment is related to
     * the n+1^th advertised audio chunk or vice versa */
    int64_t start_time = 0, lead = 0;
    for( int i = 0; i < 3; i++ )
    {
        sms = SMS_GET_SELECTED_ST( index_to_es_cat( i ) );
        if( sms && vlc_array_count( sms->chunks ) )
        {
            chunk = vlc_array_item_at_index( sms->chunks, 0 );
            p_sys->download.lead[i] = chunk->start_time + p_sys->timescale / 1000;
            if( !start_time )
                start_time = chunk->start_time;
            if( Download( s, sms ) != VLC_SUCCESS )
                goto cancel;
        }
    }
    while( 1 )
    {
        /* XXX replace magic number 10 by a value depending on
         * LookAheadFragmentCount and DVRWindowLength */
        vlc_mutex_lock( &p_sys->download.lock_wait );
        if( p_sys->b_close )
        {
            vlc_mutex_unlock( &p_sys->download.lock_wait );
            break;
        }
        lead = get_lead( s );
        while( lead > 10 * p_sys->timescale + start_time || NO_MORE_CHUNKS )
        {
            vlc_cond_wait( &p_sys->download.wait, &p_sys->download.lock_wait );
            lead = get_lead( s );
            if( p_sys->b_close )
                break;
        }
        if( p_sys->b_tseek )
        {
            int count = vlc_array_count( p_sys->download.chunks );
            chunk_t *ck = NULL;
            for( int i = 0; i < count; i++ )
            {
                ck = vlc_array_item_at_index( p_sys->download.chunks, i );
                if( unlikely( !ck ) )
                    goto cancel;
                ck->read_pos = 0;
                if( ck->data == NULL )
                    continue;
                FREENULL( ck->data );
            }
            vlc_array_destroy( p_sys->download.chunks );
            p_sys->download.chunks = vlc_array_new();
            p_sys->playback.toffset = p_sys->time_pos;
            for( int i = 0; i < 3; i++ )
            {
                p_sys->download.lead[i] = p_sys->time_pos;
                p_sys->download.ck_index[i] = 0;
            }
            p_sys->download.next_chunk_offset = 0;
            p_sys->playback.boffset = 0;
            p_sys->playback.index = 0;
            chunk_t *new_init_ck = build_init_chunk( s );
            if( !new_init_ck )
                goto cancel;
            new_init_ck->offset = p_sys->download.next_chunk_offset;
            p_sys->download.next_chunk_offset += new_init_ck->size;
            vlc_array_append( p_sys->download.chunks, new_init_ck );
            vlc_array_append( p_sys->init_chunks, new_init_ck );
            p_sys->b_tseek = false;
        }
        vlc_mutex_unlock( &p_sys->download.lock_wait );
        sms = SMS_GET_SELECTED_ST( next_track( s ) );
        if ( vlc_array_count( sms->chunks ) )
        {
            if( Download( s, sms ) != VLC_SUCCESS )
                goto cancel;
        }
    }
cancel:
    p_sys->b_error = true;
    msg_Warn(s, "Canceling download thread!");
    vlc_restorecancel( canc );
    return NULL;
}
